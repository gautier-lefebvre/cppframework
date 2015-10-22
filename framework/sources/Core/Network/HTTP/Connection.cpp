#include  <curl/curl.h>
#include  <cstring>

#include  "Library/Tool/Converter.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/String.hh"
#include  "Core/Network/HTTP/Connection.hh"
// #include  "Core/Worker/Manager.hh"
#include  "Core/Network/Exception.hh"

Core::Network::HTTP::Connection::Connection(const std::string& host, uint16_t port, uint16_t sport, const std::string& user_agent):
  Threading::Lock(),
  AEndable(),
  _host(host),
  _port(port),
  _secureport(sport),
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
        Core::Network::HTTP::Request::returnToPool(this->_pendingRequests.front());
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
    throw Core::Exception(e.what());
  }
}

const std::string&  Core::Network::HTTP::Connection::getHost(void) const {
  return this->_host;
}

uint16_t      Core::Network::HTTP::Connection::getPort(void) const {
  return this->_port;
}

uint16_t      Core::Network::HTTP::Connection::getSecurePort(void) const {
  return this->_port;
}

void  Core::Network::HTTP::Connection::addRequest(::Core::Network::HTTP::Request *request) {
  if (request != nullptr) {
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
      this->sendRequest(request);
      Core::Network::HTTP::Request::returnToPool(request);
    }
  }
}

void  Core::Network::HTTP::Connection::sendRequest(const ::Core::Network::HTTP::Request *request) const {
  Core::Network::HTTP::Response *response = nullptr;
  // Core::Network::HTTP::Callback *callback = nullptr;
  try {
    response = this->exec(request);
  } catch (const Core::Exception& e) {
    response = Core::Network::HTTP::Response::getFromPool();
    response->status = 400;
    response->reason = e.what();
  }
  if (response->status >= 400) {
    // callback = Core::Network::HTTP::Callback::get().create(request->error, request->clean, response);
  } else {
    // callback = Core::Network::HTTP::Callback::get().create(request->success, request->clean, response);
  }
  // Core::WorkerManager::get().add(callback);
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


Core::Network::HTTP::Response* Core::Network::HTTP::Connection::exec(const Core::Network::HTTP::Request *request) const {
  Core::Network::HTTP::Response  *response = Core::Network::HTTP::Response::getFromPool();
  CURLcode  result;
  std::string  protocol;
  uint16_t  port;
  CURL*    handle = curl_easy_init();
  curl_slist  *headers = NULL, *tmp;
  try {
    if (!handle) {
      throw Core::Exception("curl_easy_init failed");
    }
    protocol = request->secure ? "https" : "http";
    port = request->secure ? this->_secureport : this->_port;
    curl_easy_setopt(handle, CURLOPT_USERAGENT, this->_userAgent.c_str());
    curl_easy_setopt(handle, CURLOPT_URL, std::string(protocol + "://" + this->_host + request->url).c_str());
    curl_easy_setopt(handle, CURLOPT_PORT, static_cast<long>(port));
    if (request->body->getSize() > 0) {
      curl_easy_setopt(handle, CURLOPT_POSTFIELDS, request->body->getBytes());
      curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, request->body->getSize());
    }
    if (request->method == "GET") {
      curl_easy_setopt(handle, CURLOPT_HTTPGET, 1L);
    } else if (request->method == "POST") {
      curl_easy_setopt(handle, CURLOPT_POST, 1L);
    } else if (request->method == "HEAD") {
      curl_easy_setopt(handle, CURLOPT_NOBODY, 1L);
    } else {
      curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, request->method.c_str());
    }
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, response);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, response);
    if (!request->headers.empty()) {
      for (auto &header : request->headers) {
        if ((tmp = curl_slist_append(headers, std::string(header.first + ": " + header.second).c_str())) == NULL) {
          throw Core::Exception("curl_slist_append: fail");
        }
        headers = tmp;
      }
      curl_easy_setopt(handle, CURLOPT_HTTPHEADER, headers);
    }
    if ((result = curl_easy_perform(handle)) != CURLE_OK) {
      throw Core::Exception(std::string("curl_easy_perform: ") + curl_easy_strerror(result));
    }
    long status = 0;
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &status);
    response->status = static_cast<uint32_t>(status);
    if (headers != NULL) {
      curl_slist_free_all(headers);
    }
    curl_easy_cleanup(handle);
    return response;
  } catch (const std::exception& e) {
    if (headers != NULL) {
      curl_slist_free_all(headers);
    }
    if (handle != NULL) {
      curl_easy_cleanup(handle);
    }
    Core::Network::HTTP::Response::returnToPool(response);
    throw Core::Exception(e.what());
  }
}

// if () { /* to upload data */
//   Core::Network::HTTP::Connection::upload_object ul;
//   ul.ptr = request->body->getBuffer();
//   ul.length = request->body->getSize();
//   /* curl_easy_setopt(handle, CURLOPT_PUT, 1L); */
//   curl_easy_setopt(handle, CURLOPT_UPLOAD, 1L);
//   curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
//   curl_easy_setopt(handle, CURLOPT_READFUNCTION, read_callback);
//   curl_easy_setopt(handle, CURLOPT_READDATA, &ul);
//   curl_easy_setopt(handle, CURLOPT_INFILESIZE, static_cast<long>(ul.length));
// }