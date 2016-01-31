#ifndef   __CORE_NETWORK_HTTP_PIPELININGCONNECTION_HH__
#define   __CORE_NETWORK_HTTP_PIPELININGCONNECTION_HH__

#include  <unordered_map>
#include  <tuple>

#include  "Library/Network/CURL/MultiHandle.hh"
#include  "Core/Network/HTTP/Connection.hh"

namespace     Core {
  namespace   Network {
    namespace HTTP {
      class   PipeliningConnection :public Connection {
      private:
        typedef std::unordered_map<curlxx::EasyHandle*, std::tuple<Core::Network::HTTP::Request*, Core::Network::HTTP::Response*, bool>> HandlesMap;

      public:
        PipeliningConnection(const std::string &hostname, uint16_t port, Protocol protocol, const std::string& userAgent);
        virtual ~PipeliningConnection(void);

      public:
        /**
         *  \brief Starts the thread and starts sending pending requests.
         */
        virtual void run(void);

        virtual void routine(void);

      private:
        void  sendPipeline(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_PIPELININGCONNECTION_HH__ */
