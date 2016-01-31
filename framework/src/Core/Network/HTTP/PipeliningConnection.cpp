#include  <unistd.h>

#include  "Library/Network/CURL/Exception.hh"
#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/HTTP/PipeliningConnection.hh"
#include  "Core/Network/Exception.hh"
#include  "Core/Worker/Manager.hh"

Core::Network::HTTP::PipeliningConnection::PipeliningConnection(const std::string &hostname, uint16_t port, Protocol protocol, const std::string& userAgent):
  Core::Network::HTTP::Connection(hostname, port, protocol, userAgent)
{}

Core::Network::HTTP::PipeliningConnection::~PipeliningConnection(void) {}

void  Core::Network::HTTP::PipeliningConnection::run(void) {
  SCOPELOCK(this);
  try {
    this->_thread = new std::thread(&Core::Network::HTTP::PipeliningConnection::routine, this);
  } catch (const std::system_error& e) {
    throw Core::Network::Exception(e.what());
  }
}

void Core::Network::HTTP::PipeliningConnection::routine(void) {
  HandlesMap pipelined;
  Core::Network::HTTP::Request* request;
  Core::Network::HTTP::Response* response;
  curlxx::MultiHandle multiHandle;
  curlxx::EasyHandle* easyHandle;

  while (!(this->mustEnd())) {
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
          response = Core::Network::HTTP::Response::getFromPool();

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

    // add responses to the worker manager or wake waiting threads
    for (auto& it : pipelined) {
      try {
        request = std::get<0>(it.second);
        response = std::get<1>(it.second);

        if (request->asynchronous.isAsynchronous) {
          // asynchronous request
          // add http task and return request to pool
          if (response->status >= 400) {
            Core::Worker::Manager::get().addHTTPTask(request->error, request->clean, response);
          } else {
            Core::Worker::Manager::get().addHTTPTask(request->success, request->clean, response);
          }
          Core::Network::HTTP::Request::returnToPool(request);
        } else {
          request->wake(response);
        }
      } catch (const std::exception& e) {
        CRITICAL(e.what());
      }
    }

    multiHandle.cleanup();
  }
}

void  Core::Network::HTTP::PipeliningConnection::sendPipeline(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) {
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

      std::get<2>(it.second) = true;
      std::get<1>(it.second)->status = 400;
      std::get<1>(it.second)->reason = e.what();
    }
  }

  try {
    if (multiHandle.hasHandles()) {
      // go
      timeval interval;
      fd_set  rset, wset, eset;
      int     maxfd;
      while (multiHandle.perform()) {
        multiHandle.timeout(interval);
        multiHandle.fdset(&rset, &wset, &eset, maxfd);

        if (maxfd == -1) {
          usleep(100);
        } else {
          select(maxfd + 1, &rset, &wset, &eset, &interval);
        }
      }

      // get all status
      CURLMsg* msg;
      while ((msg = multiHandle.infoRead()) != NULL) {
        if (msg->msg == CURLMSG_DONE) {
          curlxx::EasyHandle* easyHandle = multiHandle.findHandle(msg->easy_handle);
          if (easyHandle != nullptr) {
            std::get<1>(pipelined[easyHandle])->status = easyHandle->getStatus();
            std::get<2>(pipelined[easyHandle]) = true;
          }
        }
      }
    }
  } catch (const std::exception& e) {
    CRITICAL(e.what());
  }

  // cleanup every easy handle
  // if any requests still pending (= fail)
  // set general error status
  for (auto& it : pipelined) {
    if (std::get<2>(it.second) == false) {
      std::get<1>(it.second)->status = 400;
      std::get<1>(it.second)->reason = "HTTP pipelining failed";
    }

    curlxx::EasyHandle::returnToPool(it.first);
  }
}
