#ifndef   __CORE_NETWORK_HTTP_CLIENT_HH__
#define   __CORE_NETWORK_HTTP_CLIENT_HH__

#include  <vector>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Core/Network/HTTP/Connection.hh"

namespace     Core {
  namespace   Network {
    /**
     *  \namespace HTTP
     *  \brief All HTTP classes (requests / responses / client).
     */
    namespace HTTP {
      /**
       *  \class Client Core/Network/HTTP/Client.hh
       *  \brief Singleton class handling HTTP connections.
       */
      class Client: public Singleton<Core::Network::HTTP::Client>, public Threading::Lockable,         public AEndable {
        friend class Singleton<Core::Network::HTTP::Client>;
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
         *  \brief Gets a connection from its hostname, port and secure port.
         *  \param hostname the hostname of the HTTP server.
         *  \param port the port of the HTTP server.
         *  \param securePort the secure port (https) of the HTTP server.
         *  \param create if true and the connection is not found, creates it.
         *  \return nullptr if the connection is not found and create is false, else return the connection.
         */
        Connection* getConnectionByHostPort(const std::string& hostname, uint16_t port = 80, uint16_t securePort = 443, bool create = true);

        /**
         *  \brief Send a request to a specific HTTP server. If a connection to this server is not already open, opens it.
         *  \param request the request to send.
         *  \param hostname the hostname of the HTTP server.
         *  \param port the port of the HTTP server.
         *  \param securePort the secure port (https) of the HTTP server.
         */
        void  sendRequest(Core::Network::HTTP::Request *request, const std::string& hostname, uint16_t port = 80, uint16_t securePort = 443);

        /**
         *  \brief Send a request to a specific open HTTP server.
         *  \param request the request to send.
         *  \param connection the open connection to the HTTP server.
         */
        void  sendRequest(Core::Network::HTTP::Request *request, Core::Network::HTTP::Connection *connection);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_CLIENT_HH__ */
