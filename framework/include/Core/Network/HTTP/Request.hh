#ifndef   __CORE_NETWORK_HTTP_REQUEST_HH__
#define   __CORE_NETWORK_HTTP_REQUEST_HH__

#include  <functional>

#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/HTTP/AMessage.hh"
#include  "Core/Network/HTTP/Response.hh"

namespace fwk {
  namespace Core {
    namespace Network {
      namespace HTTP {
        /**
         *  \class Request Core/Network/HTTP/Request.hh
         *  \brief A request to send to a HTTP server.
         */
        struct Request :public AMessage, public Factory::TPooled<Core::Network::HTTP::Request, 10, 2> {
          std::string method; /*!< method (or verb) of the request. */
          std::string url; /*!< url of the request (ex: "/posts"). */
          std::function<void (const Core::Network::HTTP::Response *)> success; /*!< callback called when the response has a status < 400. */
          std::function<void (const Core::Network::HTTP::Response *)> error; /*!< callback called when the response >= 400. */
          std::function<void (void)> clean; /*!< callback called when the response is not valid. */

          struct {
            bool isFile; /*!< wether of not the request must upload a file. */
            std::string filepath; /*!< path of the file to upload. */
          } file; /*!< information on a file to send. */

          struct {
            bool isAsynchronous; /*!< wether or not the request is synchronous. The other attributes of this structure are invalid if this variable is true. */
            bool isValid; /*!< if false, the thread was only woken because the connection is being closed. */
            Threading::Condition lock; /*!< condition variable to wait on until the response is read. */
            Core::Network::HTTP::Response* response; /*!< the response to the request. This is only set in case of a synchronous request. */
          } asynchronous; /*!< information on a synchronous request. */

          /**
           *  \brief Constructor of Request.
           */
          Request(void);

          /**
           *  \brief Destructor of Request.
           */
          virtual ~Request(void);

          /**
           *  \brief Reinits the request. Returns the synchronous response to the pool.
           */
          virtual void  reinit(void);

          /**
           *  \brief Returns the synchronous response to the pool.
           */
          void  cleanResponse(void);

          /**
           *  \brief Waits for the synchronous request to end. Never returns if the request was not set synchronous.
           *  \return true if the request received a response. false if the condition variable was notified because the connection is being closed.
           */
          bool  wait(void);

          /**
           *  \brief Wakes the synchronous request.
           *  \param response the http response read. If nullptr, means that the response was not read and the thread was only woken because the client is being shut down.
           */
          void  wake(Core::Network::HTTP::Response* response);

          /**
            synchronous request usage:

            Core::Network::HTTP::Request* request = Core::Network::HTTP::Request::getFromPool();
            request->method = ...;
            request->url = ...;
            ...

            request->asynchronous.isAsynchronous = false;

            Core::Network::HTTP::Client::get().sendRequest(request, hostname, [port, secureport]);

            if (request->wait()) { handle response}
            else { clean (no response) }

            Core::Network::HTTP::Request::returnToPool(request);
          */
        };
      }
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_REQUEST_HH__ */
