#ifndef   __CORE_NETWORK_HTTP_PIPELININGCONNECTION_HH__
#define   __CORE_NETWORK_HTTP_PIPELININGCONNECTION_HH__

#include  <unordered_map>
#include  <tuple>

#include  "Library/Network/CURL/MultiHandle.hh"
#include  "Core/Network/HTTP/Connection.hh"

namespace     Core {
  namespace   Network {
    namespace HTTP {
      /**
       *  \class PipeliningConnection Core/Network/HTTP/PipeliningConnection.hh
       *  \brief A class running on a thread and managing a queue of requests to a HTTP server, pipelining when it can.
       */
      class   PipeliningConnection :public Connection {
      private:
        typedef std::unordered_map<curlxx::EasyHandle*, std::tuple<Core::Network::HTTP::Request*, Core::Network::HTTP::Response*, bool>> HandlesMap;

      public:
        /**
         *  \brief Constructor of PipeliningConnection.
         *  \param hostname the hostname of the HTTP server.
         *  \param port the port of the HTTP server.
         *  \param protocol the protocol used (HTTP/HTTPS/FTP...).
         *  \param userAgent the user agent string used when sending requests.
         */
        PipeliningConnection(const std::string &hostname, uint16_t port, Protocol protocol, const std::string& userAgent);

        /**
         *  \brief Destructor of PipeliningConnection.
         */
        virtual ~PipeliningConnection(void);

      public:
        /**
         *  \brief Starts the thread and starts sending pending requests.
         */
        virtual void run(void);

      private:
        /**
         *  \brief The internal routine of the thread. Pipelines the requests and handles the responses.
         */
        void routine(void);

      private:
        /**
         *  \brief Sends multiple requests to a server and handle their answers.
         *  Any error on a request will generate a generic error.
         *  \param pipelined the list of easy handles linked to their requests / responses.
         *  \param multiHandle see CURL man, the handle which will manage the given easy handles.
         */
        void  sendPipeline(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const;

        /**
         *  \brief Reads all pending messages received on the multi handle.
         *  Adds HTTP tasks to the worker manager (asynchronous requests) or wakes the waiting threads (synchronous requests).
         *  \param pipelined the list of easy handles linked to their requests / responses.
         *  \param multiHandle see CURL man, the handle which will manage the given easy handles.
         */
        void  getAnswers(HandlesMap& pipelined, curlxx::MultiHandle& multiHandle) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_PIPELININGCONNECTION_HH__ */
