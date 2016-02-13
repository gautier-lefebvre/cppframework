#include  <curl/curl.h>
#include  <sys/stat.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <cstring>

#include  "Library/Network/CURL/EasyHandle.hh"
#include  "Library/Network/CURL/Method.hh"
#include  "Library/Network/CURL/Exception.hh"
#include  "Library/Tool/Converter.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/String.hh"
#include  "Core/Network/Http/HttpConnection.hh"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

HttpConnection::HttpConnection(const std::string& host, uint16_t port, HttpProtocol protocol, const std::string& user_agent):
  Lockable(),
  AEndable(),
  _host(host),
  _port(port),
  _protocol(protocol),
  _userAgent(user_agent),
  _thread(nullptr),
  _pendingRequests()
{}

HttpConnection::~HttpConnection(void) {
  this->end();
}

void  HttpConnection::end(void) {
  SCOPELOCK(this);
  if (!(this->mustEnd())) {
    {
      ScopeLock sl(this->_pendingRequests);
      this->_end = true;
      this->_pendingRequests.notify_all();
    }

    if (this->_thread) {
      try {
        this->_thread->join();
        delete this->_thread;
        this->_thread = nullptr;
      } catch (const std::system_error& e) {
        ERROR(e.what());
      }
    }

    {
      ScopeLock slrequest(this->_pendingRequests);
      while (!(this->_pendingRequests.empty())) {
        HttpRequest* request = this->_pendingRequests.front();
        this->setResponse(request, nullptr);
        this->_pendingRequests.pop();
      }
    }
  }
}

void  HttpConnection::run(void) {
  SCOPELOCK(this);
  try {
    if (this->_thread == nullptr) {
      this->_thread = new std::thread(&HttpConnection::routine, this);
    }
  } catch (const std::system_error& e) {
    throw NetworkException(e.what());
  }
}

const std::string&  HttpConnection::getHost(void) const {
  return this->_host;
}

uint16_t      HttpConnection::getPort(void) const {
  return this->_port;
}

HttpProtocol HttpConnection::getProtocol(void) const {
  return this->_protocol;
}

void  HttpConnection::addRequest(::HttpRequest *request) {
  if (request != nullptr) {
    if (!request->asynchronous.isAsynchronous) {
      request->asynchronous.isValid = true;
    }

    SCOPELOCK(&(this->_pendingRequests));
    this->_pendingRequests.push(request);
    this->_pendingRequests.notify();
  }
}

void  HttpConnection::routine(void) {
  ::HttpRequest *request;
  while (!(this->mustEnd())) {
    request = nullptr;

    {
      SCOPELOCK(&(this->_pendingRequests));
      if (this->_pendingRequests.empty()) {
        this->_pendingRequests.wait();
      } else {
        request = this->_pendingRequests.front();
        this->_pendingRequests.pop();
      }
    }

    if (request != nullptr) {
      HttpResponse* response = this->sendRequest(request);
      this->setResponse(request, response);
    }
  }
}

HttpResponse*  HttpConnection::sendRequest(const ::HttpRequest *request) const {
  HttpResponse *response = nullptr;

  try {
    response = this->exec(request);
  } catch (const NetworkException& e) {
    response = HttpResponse::getFromPool();
    response->status = 400;
    response->reason = e.what();
  }

  return response;
}

void HttpConnection::prepareHandle(curlxx::EasyHandle* handle, const HttpRequest *request, HttpResponse* response) const {
  try {
    handle->init();

    handle->setUserAgent(this->_userAgent);
    handle->setURL(HttpProtocolToString.key.at(this->_protocol) + "://" + this->_host + request->url);
    handle->setPort(this->_port);
    handle->setMethod(request->method);
    handle->setHeaders(request->headers);

    // set body or set file
    if (request->file.isFile) {
      handle->setFile(request->file.filepath, &read_callback);
    } else if (request->body->getSize() > 0) {
      handle->setBody(request->body->atStart(), request->body->getSize());
    }

    // set response callbacks
    response->init();
    handle->setResponseCallbacks(response, &write_callback, &header_callback);
  } catch (const std::exception& e) {
    throw NetworkException(e.what());
  }
}

curlxx::EasyHandle* HttpConnection::prepareHandle(const HttpRequest *request, HttpResponse* response) const {
  curlxx::EasyHandle* handle = curlxx::EasyHandle::getFromPool();

  try {
    this->prepareHandle(handle, request, response);
  } catch (const std::exception&) {
    curlxx::EasyHandle::returnToPool(handle);
    throw;
  }

  return handle;
}

HttpResponse* HttpConnection::exec(const HttpRequest *request) const {
  HttpResponse* response = HttpResponse::getFromPool();
  curlxx::EasyHandle* handle = nullptr;

  try {
    handle = this->prepareHandle(request, response);

    // send request and get status
    handle->perform();
    response->status = handle->getStatus();

    curlxx::EasyHandle::returnToPool(handle);

    return response;
  } catch (const std::exception& e) {
    HttpResponse::returnToPool(response);
    curlxx::EasyHandle::returnToPool(handle);
    throw NetworkException(e.what());
  }

}

void HttpConnection::setResponse(HttpRequest* request, HttpResponse* response) const {
  if (request->asynchronous.isAsynchronous) {
    // asynchronous request
    // add http task and return request to pool
    if (response) {
      if (response->status >= 400) {
        WorkerManager::get().addHttpTask(request->error, request->clean, response);
      } else {
        WorkerManager::get().addHttpTask(request->success, request->clean, response);
      }
    }
    HttpRequest::returnToPool(request);
  } else {
    request->wake(response);
  }
}

size_t  HttpConnection::read_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  HttpConnection::upload_object *u = reinterpret_cast<HttpConnection::upload_object*>(userdata);

  size_t copy_size = (u->length < size * nmemb ? u->length : size * nmemb);
  memcpy(data, u->ptr, copy_size);
  u->ptr += copy_size;
  u->length -= copy_size;
  return copy_size;
}

size_t  HttpConnection::header_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  HttpResponse *response = reinterpret_cast<HttpResponse*>(userdata);
  std::string header(reinterpret_cast<char*>(data), size * nmemb);
  size_t separator = header.find_first_of(":");

  if (separator != std::string::npos) {
    std::string key = header.substr(0, separator);
    std::string value = header.substr(separator + 1);
    trim(key);
    trim(value);
    response->headers[key] = value;
  } else {
    trim(header);

    if (header.length() == 0) {
      return size * nmemb;
    } else {
      response->headers[header] = "present";
    }
  }

  return size * nmemb;
}

size_t  HttpConnection::write_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  HttpResponse *response = reinterpret_cast<HttpResponse*>(userdata);
  response->body->push(reinterpret_cast<const uint8_t*>(data), size * nmemb, true);
  return size * nmemb;
}
