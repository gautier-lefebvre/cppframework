#ifndef   __CORE_NETWORK_HTTP_HTTPCLIENT_HH__
#define   __CORE_NETWORK_HTTP_HTTPCLIENT_HH__

#include  <vector>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Core/Network/Http/HttpConnection.hh"

namespace fwk {
    /**
     *  \class HttpClient Core/Network/Http/HttpClient.hh
     *  \brief Singleton class handling HTTP connections.
     */
    class HttpClient: public Singleton<fwk::HttpClient>, public Lockable, public AEndable {
        friend class Singleton<fwk::HttpClient>;
    private:
        std::string _userAgent; /*!< user agent string sent for each request. */
        std::vector<HttpConnection*> _connections; /*!< open http connections. */

    public:
        static const std::string defaultUserAgent; /*!< default user agent string used when the user agent string is empty. */

    private:
        /**
         *  \brief Deleted copy constructor of HttpClient.
         */
        HttpClient(const HttpClient&) = delete;

        /**
         *  \brief Deleted move constructor of HttpClient.
         */
        HttpClient(const HttpClient&&) = delete;

        /**
         *  \brief Deleted assignment constructor of HttpClient.
         */
        HttpClient& operator=(const HttpClient&) = delete;

    private:
        /**
         *  \brief Constructor of HttpClient.
         */
        HttpClient(void);

        /**
         *  \brief Destructor of HttpClient.
         */
        virtual ~HttpClient(void);

    public:
        /**
         *  \brief Closes every open HTTP connections and clears pending requests.
         *  Wakes any thread waiting on a synchronous request.
         */
        virtual void  onEnd(void);

    public:
        /**
         *  \brief Sets the user agent string. Does not change the user agent of open connections.
         *  \param user_agent the user agent string used for every new HTTP connection.
         */
        void  init(const std::string& user_agent);

        /**
         *  \brief Creates a new connection. Requests will not be sent until connection->run() is called.
         *
         *  If a connection to this host/port/protocol already exists,
         *  this returns the existing connection, but does not change the pipelining parameter.
         *
         *  \param hostname hostname of the HTTP server.
         *  \param port port of the HTTP server.
         *  \param protocol protocol used (HTTP/HTTPS/FTP).
         *  \param enablePipelining true to enable HTTP pipelining.
         *  \return the connection created.
         */
        HttpConnection*  initConnection(const std::string& hostname, uint16_t port = 80, HttpProtocol protocol = HttpProtocol::HTTP, bool enablePipelining = true);

        /**
         *  \brief Send a request to a specific open HTTP server.
         *  \param request the request to send.
         *  \param connection the open connection to the HTTP server.
         */
        void  sendRequest(HttpConnection *connection, HttpRequest *request);
    };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPCLIENT_HH__ */
