#ifndef   __CORE_NETWORK_HTTP_CLIENT_HH__
#define   __CORE_NETWORK_HTTP_CLIENT_HH__

#include  <vector>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Core/Network/HTTP/Connection.hh"

namespace fwk {
  namespace Core {
    namespace Network {
      /**
       *  \namespace Core::Network::HTTP
       *  \brief All HTTP classes (requests / responses / client).
       */
      namespace HTTP {
        /**
         *  \class Client Core/Network/HTTP/Client.hh
         *  \brief Singleton class handling HTTP connections.
         */
        class Client: public Singleton<fwk::Core::Network::HTTP::Client>, public Threading::Lockable, public AEndable {
          friend class Singleton<fwk::Core::Network::HTTP::Client>;
        private:
          std::string _userAgent; /*!< user agent string sent for each request. */
          std::vector<Connection*> _connections; /*!< open http connections. */

        public:
          static const std::string defaultUserAgent; /*!< default user agent string used when the user agent string is empty. */

        private:
          /**
           *  \brief Deleted copy constructor of Client.
           */
          Client(const Client&) = delete;

          /**
           *  \brief Deleted move constructor of Client.
           */
          Client(const Client&&) = delete;

          /**
           *  \brief Deleted assignment constructor of Client.
           */
          Client& operator=(const Client&) = delete;

        private:
          /**
           *  \brief Constructor of Client.
           */
          Client(void);

          /**
           *  \brief Destructor of Client.
           */
          virtual ~Client(void);

        public:
          /**
           *  \brief Closes every open HTTP connections and clears pending requests.
           *  Wakes any thread waiting on a synchronous request.
           */
          virtual void  end(void);

        public:
          /**
           *  \brief Sets the user agent string. Does not change the user agent of open connections.
           *  \param user_agent the user agent string used for every new HTTP connection.
           */
          void  init(const std::string& user_agent);

          /**
           *  \brief Creates a new connection.
           *
           *  If a connection to this host/port/protocol already exists,
           *  this returns the existing connection, but does not change the pipelining parameter.
           *
           *  \throw Core::Network::Exception if the connection to the server could not be initialized (i.e. the thread did not launch).
           *  \param hostname hostname of the HTTP server.
           *  \param port port of the HTTP server.
           *  \param protocol protocol used (HTTP/HTTPS/FTP).
           *  \param enablePipelining true to enable HTTP pipelining.
           *  \return the connection created.
           */
          Connection*  initConnection(const std::string& hostname, uint16_t port = 80, Core::Network::HTTP::Protocol protocol = Core::Network::HTTP::Protocol::HTTP, bool enablePipelining = true);

          /**
           *  \brief Send a request to a specific open HTTP server.
           *  \param request the request to send.
           *  \param connection the open connection to the HTTP server.
           */
          void  sendRequest(Core::Network::HTTP::Connection *connection, Core::Network::HTTP::Request *request);
        };
      }
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_CLIENT_HH__ */
