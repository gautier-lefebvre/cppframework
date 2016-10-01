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

void  HttpConnection::onEnd(void) {
    SCOPELOCK(this);

    {
        ScopeLock sl(this->_pendingRequests);
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
        ScopeLock sl(this->_pendingRequests);
        while (!(this->_pendingRequests.empty())) {
            HttpRequest* request = this->_pendingRequests.front();
            this->setResponse(request, nullptr);
            this->_pendingRequests.pop();
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
    while (!(this->isEnding())) {
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
        response->init();
        response->_isValid = false;
        response->status = static_cast<uint32_t>(HttpResponse::Status::INVALID_RESPONSE);
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
        handle->enableFollowLocation(true);

        // set body or set file
        if (request->file.isFile) {
            handle->setFile(request->file.filepath, &read_callback);
        } else if (request->body->getSize() > 0) {
            handle->setBody(request->body->atStart(), request->body->getSize());
        }

        // set response callbacks
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
    response->init();

    curlxx::EasyHandle* handle = nullptr;

    try {
        handle = this->prepareHandle(request, response);

        // send request and get status
        handle->perform();
        response->status = handle->getStatus();
        response->_isValid = true;

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
            auto& requestCallback = response->status >= 400 ? request->error : request->success;

            if (!request->clean && !requestCallback) {
                // if no callbacks to call, clear the response.
                HttpResponse::returnToPool(response);
            } else {
                // else add a task to the queue.
                std::function<void (void)> taskCallback = nullptr;
                std::function<void (void)> cleanup = [=] (void) -> void {
                    try {
                        if (request->clean) {
                            request->clean();
                        }

                        HttpResponse::returnToPool(response);
                    } catch (const std::exception&) {
                        HttpResponse::returnToPool(response);
                        throw;
                    }
                };

                // only create the response lambda if a callback was specified.
                if (requestCallback) {
                    taskCallback = [=] (void) -> void {
                        requestCallback(response);
                    };
                }

                // no need to keep a key for this task
                WorkerManager::get().addSimpleTask(nullptr, taskCallback, cleanup);
            }
        }
        HttpRequest::returnToPool(request);
    } else {
        request->wake(response);
    }
}

size_t  HttpConnection::read_callback(void *data, size_t size, size_t nmemb, void *userdata) {
    // I totally did not copy this from the internet.
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

        // todo use a regex ? the http reason is not really useful
        // no headers yet -> status line
        if (response->headers.empty()) {
            size_t i, spaces = 0;
            for (i = 0; i < header.length() - 1; ++i) {
                // 2 spaces before "HTTPN/M STATUS {REASON}"
                if (header[i] == ' ' && ++spaces >= 2) {
                    response->reason = std::string(header.c_str() + i + 1);
                    break;
                }
            }
        }

        if (header.length() == 0) {
            return size * nmemb;
        } else {
            response->headers[header] = "";
        }
    }

    return size * nmemb;
}

size_t  HttpConnection::write_callback(void *data, size_t size, size_t nmemb, void *userdata) {
    HttpResponse *response = reinterpret_cast<HttpResponse*>(userdata);
    response->body->push(reinterpret_cast<const uint8_t*>(data), size * nmemb, true);
    return size * nmemb;
}
