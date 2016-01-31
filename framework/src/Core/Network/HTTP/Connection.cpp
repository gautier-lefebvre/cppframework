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
#include  "Core/Network/HTTP/Connection.hh"
#include  "Core/Worker/Manager.hh"
#include  "Core/Network/Exception.hh"

Core::Network::HTTP::Connection::Connection(const std::string& host, uint16_t port, Core::Network::HTTP::Protocol protocol, const std::string& user_agent):
  Threading::Lockable(),
  AEndable(),
  _host(host),
  _port(port),
  _protocol(protocol),
  _userAgent(user_agent),
  _thread(nullptr),
  _pendingRequests()
{}

Core::Network::HTTP::Connection::~Connection(void) {
  this->end();
}

void  Core::Network::HTTP::Connection::end(void) {
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
      } catch (const std::system_error& e) {
        ERROR(e.what());
      }
    }

    {
      ScopeLock slrequest(this->_pendingRequests);
      while (!(this->_pendingRequests.empty())) {
        Core::Network::HTTP::Request* request = this->_pendingRequests.front();
        this->setResponse(request, nullptr);
        this->_pendingRequests.pop();
      }
    }
  }
}

void  Core::Network::HTTP::Connection::run(void) {
  SCOPELOCK(this);
  try {
    this->_thread = new std::thread(&Core::Network::HTTP::Connection::routine, this);
  } catch (const std::system_error& e) {
    throw Core::Network::Exception(e.what());
  }
}

const std::string&  Core::Network::HTTP::Connection::getHost(void) const {
  return this->_host;
}

uint16_t      Core::Network::HTTP::Connection::getPort(void) const {
  return this->_port;
}

Core::Network::HTTP::Protocol Core::Network::HTTP::Connection::getProtocol(void) const {
  return this->_protocol;
}

void  Core::Network::HTTP::Connection::addRequest(::Core::Network::HTTP::Request *request) {
  if (request != nullptr) {
    if (!request->asynchronous.isAsynchronous) {
      request->asynchronous.isValid = true;
    }

    SCOPELOCK(&(this->_pendingRequests));
    this->_pendingRequests.push(request);
    this->_pendingRequests.notify();
  }
}

void  Core::Network::HTTP::Connection::routine(void) {
  ::Core::Network::HTTP::Request *request;
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
      Core::Network::HTTP::Response* response = this->sendRequest(request);
      this->setResponse(request, response);
    }
  }
}

Core::Network::HTTP::Response*  Core::Network::HTTP::Connection::sendRequest(const ::Core::Network::HTTP::Request *request) const {
  Core::Network::HTTP::Response *response = nullptr;

  try {
    response = this->exec(request);
  } catch (const Core::Network::Exception& e) {
    response = Core::Network::HTTP::Response::getFromPool();
    response->status = 400;
    response->reason = e.what();
  }

  return response;
}

void Core::Network::HTTP::Connection::prepareHandle(curlxx::EasyHandle* handle, const Core::Network::HTTP::Request *request, Core::Network::HTTP::Response* response) const {
  try {
    handle->init();

    handle->setUserAgent(this->_userAgent);
    handle->setURL(Core::Network::HTTP::ProtocolToString.key.at(this->_protocol) + "://" + this->_host + request->url);
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
    throw Core::Network::Exception(e.what());
  }
}

curlxx::EasyHandle* Core::Network::HTTP::Connection::prepareHandle(const Core::Network::HTTP::Request *request, Core::Network::HTTP::Response* response) const {
  curlxx::EasyHandle* handle = curlxx::EasyHandle::getFromPool();

  try {
    handle->init();

    handle->setUserAgent(this->_userAgent);
    handle->setURL(Core::Network::HTTP::ProtocolToString.key.at(this->_protocol) + "://" + this->_host + request->url);
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
    curlxx::EasyHandle::returnToPool(handle);
    throw Core::Network::Exception(e.what());
  }

  return handle;
}

Core::Network::HTTP::Response* Core::Network::HTTP::Connection::exec(const Core::Network::HTTP::Request *request) const {
  Core::Network::HTTP::Response* response = Core::Network::HTTP::Response::getFromPool();
  curlxx::EasyHandle* handle = nullptr;

  try {
    handle = this->prepareHandle(request, response);

    // send request and get status
    handle->perform();
    response->status = handle->getStatus();

    curlxx::EasyHandle::returnToPool(handle);

    return response;
  } catch (const std::exception& e) {
    Core::Network::HTTP::Response::returnToPool(response);
    curlxx::EasyHandle::returnToPool(handle);
    throw Core::Network::Exception(e.what());
  }

}

void Core::Network::HTTP::Connection::setResponse(Core::Network::HTTP::Request* request, Core::Network::HTTP::Response* response) const {
  if (request->asynchronous.isAsynchronous) {
    // asynchronous request
    // add http task and return request to pool
    if (response) {
      if (response->status >= 400) {
        Core::Worker::Manager::get().addHTTPTask(request->error, request->clean, response);
      } else {
        Core::Worker::Manager::get().addHTTPTask(request->success, request->clean, response);
      }
    }
    Core::Network::HTTP::Request::returnToPool(request);
  } else {
    request->wake(response);
  }
}

size_t  Core::Network::HTTP::Connection::read_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  Core::Network::HTTP::Connection::upload_object *u = reinterpret_cast<Core::Network::HTTP::Connection::upload_object*>(userdata);

  size_t copy_size = (u->length < size * nmemb ? u->length : size * nmemb);
  memcpy(data, u->ptr, copy_size);
  u->ptr += copy_size;
  u->length -= copy_size;
  return copy_size;
}

size_t  Core::Network::HTTP::Connection::header_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  Core::Network::HTTP::Response *response = reinterpret_cast<Core::Network::HTTP::Response*>(userdata);
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

size_t  Core::Network::HTTP::Connection::write_callback(void *data, size_t size, size_t nmemb, void *userdata) {
  Core::Network::HTTP::Response *response = reinterpret_cast<Core::Network::HTTP::Response*>(userdata);
  response->body->push(reinterpret_cast<const uint8_t*>(data), size * nmemb, true);
  return size * nmemb;
}
