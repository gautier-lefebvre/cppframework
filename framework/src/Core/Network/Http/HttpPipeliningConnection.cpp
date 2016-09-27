#include  <unistd.h>

#include  "Library/Network/CURL/Exception.hh"
#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/Http/HttpPipeliningConnection.hh"
#include  "Core/Network/Exception.hh"
#include  "Core/Worker/WorkerManager.hh"

using namespace fwk;

HttpPipeliningConnection::HttpPipeliningConnection(const std::string &hostname, uint16_t port, HttpProtocol protocol, const std::string& userAgent):
    HttpConnection(hostname, port, protocol, userAgent)
{}

HttpPipeliningConnection::~HttpPipeliningConnection(void) {}

void  HttpPipeliningConnection::run(void) {
    SCOPELOCK(this);
    try {
        this->_thread = new std::thread(&HttpPipeliningConnection::routine, this);
    } catch (const std::system_error& e) {
        throw NetworkException(e.what());
    }
}

void HttpPipeliningConnection::routine(void) {
    HandlesMap pipelined;
    HttpRequest* request;
    HttpResponse* response;
    curlxx::MultiHandle multiHandle;
    curlxx::EasyHandle* easyHandle;

    while (!(this->isEnding())) {
        pipelined.clear();

        {
            SCOPELOCK(&(this->_pendingRequests));
            if (this->_pendingRequests.empty()) {
                this->_pendingRequests.wait();
                continue;
            } else {
                for (size_t i = 0 ; i < curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB && !(this->_pendingRequests.empty()) ; ++i) {
                    easyHandle = curlxx::EasyHandle::getFromPool();

                    request = this->_pendingRequests.front();
                    response = HttpResponse::getFromPool();
                    response->init();

                    pipelined[easyHandle] = std::make_tuple(request, response, false);

                    this->_pendingRequests.pop();
                }
            }
        }

        try {
            this->sendPipeline(pipelined, multiHandle);
        } catch (const std::exception& e) {
            CRITICAL(e.what());
        }

        multiHandle.cleanup();
    }
}

void  HttpPipeliningConnection::sendPipeline(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const {
    multiHandle.init();

    // prepare all requests
    // do not add handles whose preparation failed to the multi handle
    //    and set a general error response
    for (auto& it : pipelined) {
        try {
            this->prepareHandle(it.first, std::get<0>(it.second), std::get<1>(it.second));
            multiHandle.addHandle(it.first);
        } catch (const std::exception& e) {
            CRITICAL(e.what());

            // set general error response
            std::get<2>(it.second) = true;
            std::get<1>(it.second)->_isValid = false;
            std::get<1>(it.second)->status = static_cast<uint32_t>(HttpResponse::Status::INVALID_RESPONSE);
            std::get<1>(it.second)->reason = e.what();

            // add http task or wake waiting thread
            this->setResponse(std::get<0>(it.second), std::get<1>(it.second));
        }
    }

    try {
        if (multiHandle.hasHandles()) {
            // go
            timeval interval;
            fd_set  rset, wset, eset;
            int     maxfd;
            while (multiHandle.perform()) {
                // get all status
                this->getAnswers(pipelined, multiHandle);

                multiHandle.timeout(interval);
                multiHandle.fdset(&rset, &wset, &eset, maxfd);

                if (maxfd == -1) {
                    usleep(100);
                } else {
                    select(maxfd + 1, &rset, &wset, &eset, &interval);
                }
            }

            this->getAnswers(pipelined, multiHandle);

        }
    } catch (const std::exception& e) {
        CRITICAL(e.what());
    }

    // cleanup every easy handle
    // if any requests still pending (= fail)
    // set general error status
    for (auto& it : pipelined) {
        if (std::get<2>(it.second) == false) {
            std::get<1>(it.second)->_isValid = false;
            std::get<1>(it.second)->status = static_cast<uint32_t>(HttpResponse::Status::INVALID_RESPONSE);
            std::get<1>(it.second)->reason = "HTTP pipelining failed";

            // add http job or wake waiting thread
            this->setResponse(std::get<0>(it.second), std::get<1>(it.second));
        }

        curlxx::EasyHandle::returnToPool(it.first);
    }
}

void HttpPipeliningConnection::getAnswers(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const {
    // get the answers and send the response to the worker manager
    CURLMsg* msg;
    while ((msg = multiHandle.infoRead()) != NULL) {
        if (msg->msg == CURLMSG_DONE) {
            curlxx::EasyHandle* easyHandle = multiHandle.findHandle(msg->easy_handle);
            if (easyHandle != nullptr) {
                // set status
                auto& handleData = pipelined.at(easyHandle);

                if (msg->data.result == CURLE_OK) {
                    std::get<1>(handleData)->_isValid = true;
                    std::get<1>(handleData)->status = easyHandle->getStatus();
                } else {
                    std::get<1>(handleData)->_isValid = false;
                    std::get<1>(handleData)->status = static_cast<uint32_t>(HttpResponse::Status::INVALID_RESPONSE);
                    std::get<1>(handleData)->reason = "HTTP request failed";
                }

                std::get<2>(handleData) = true;

                // add http job or wake waiting thread
                this->setResponse(std::get<0>(handleData), std::get<1>(handleData));
            }
        }
    }
}
