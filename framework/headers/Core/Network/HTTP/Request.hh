#ifndef   __CORE_NETWORK_HTTP_REQUEST_HH__
#define   __CORE_NETWORK_HTTP_REQUEST_HH__

#include  <functional>

#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/HTTP/AMessage.hh"
#include  "Core/Network/HTTP/Response.hh"

namespace     Core {
  namespace   Network {
    namespace HTTP {
      struct  Request :public AMessage, public Factory::TPooled<Core::Network::HTTP::Request, 10, 2> {
        std::string method;
        std::string url;
        bool secure;
        std::function<void (const Core::Network::HTTP::Response *)> success;
        std::function<void (const Core::Network::HTTP::Response *)> error;
        std::function<void (void)> clean;

        struct {
          bool isFile;
          std::string filepath;
        } file;

        struct {
          bool isAsynchronous;
          bool isValid;
          Threading::Condition lock;
          Core::Network::HTTP::Response* response;
        } asynchronous;

        Request(void);
        virtual ~Request(void);

        virtual void reinit(void);

        void cleanResponse(void);
        bool wait(void);

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

#endif    /* __CORE_NETWORK_HTTP_REQUEST_HH__ */
