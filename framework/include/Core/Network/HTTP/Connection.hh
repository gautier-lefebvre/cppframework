#ifndef   __CORE_NETWORK_HTTP_CONNECTION_HH__
#define   __CORE_NETWORK_HTTP_CONNECTION_HH__

#include  <queue>
#include  <thread>

#include  "Library/Network/CURL/EasyHandle.hh"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/HTTP/Request.hh"
#include  "Core/Network/HTTP/Protocol.hh"

namespace     Core {
  namespace   Network {
    namespace HTTP {
      /**
       *  \class Connection Core/Network/HTTP/Connection.hh
       *  \brief An open connection to an HTTP server.
       */
      class Connection :public Threading::Lockable, public AEndable {
      protected:
        /**
         *  \class upload_object Core/Network/HTTP/Connection.hh
         *  \brief used by CURL to send requests.
         */
        struct upload_object {
          const uint8_t*  ptr; /*!< A pointer to the Core::Network::HTTP::Response. */
          size_t      length; /*!< Number of bytes read. */
        };

      protected:
        std::string _host; /*!< hostname of the HTTP server. */
        uint16_t    _port; /*!< port of the HTTP server. */
        Protocol    _protocol; /*!< protocol of the server (HTTP/HTTPS/FTP...) */
        std::string _userAgent; /*!< user agent used for requests. */
        std::thread *_thread; /*!< thread used by the connection. */
        Threading::TNotifiable<std::queue<Core::Network::HTTP::Request*>>  _pendingRequests; /*!< pending requests. */

      public:
        /**
         *  \brief Constructor of Connection.
         *  \param hostname hostname of the HTTP server.
         *  \param port port of the HTTP server.
         *  \param protocol protocol used.
         *  \param userAgent user agent used for sending requests.
         */
        Connection(const std::string &hostname, uint16_t port, Protocol protocol, const std::string& userAgent);

        /**
         *  \brief Destructor of Connection. Calls the end method.
         */
        virtual ~Connection(void);

      public:
        /**
         *  \brief Clears pending requests. Wakes any thread waiting on a synchronous request.
         */
        virtual void  end(void);

      public:
        /**
         *  \brief Starts the thread and starts sending pending requests.
         */
        virtual void run(void);

        /**
         *  \brief Returns the hostname of the HTTP server.
         *  \return the hostname of the HTTP server.
         */
        const std::string&  getHost(void) const;

        /**
         *  \brief Returns the port of the HTTP server.
         *  \return the port of the HTTP server.
         */
        uint16_t  getPort(void) const;

        /**
         *  \brief Returns the protocol.
         *  \return the protocol.
         */
        Protocol  getProtocol(void) const;

        /**
         *  \brief Adds a request to the queue of pending requests.
         *  \param request the request to add.
         */
        void      addRequest(Core::Network::HTTP::Request *request);

      protected:
        /**
         *  \brief Send a request. If the request fails, prepares a default 400 response.
         *  \param request the request to send.
         *  \return the response.
         */
        Response* sendRequest(const Core::Network::HTTP::Request *request) const;

        /**
         *  \brief Send pending requests and waits for new ones until the end method is called.
         */
        virtual void routine(void);

        /**
         *  \brief Effectively send a request.
         *  \throw Core::Network::Exception if the request failed to send.
         *  \param request the request to send.
         *  \return the response.
         */
        Response* exec(const Core::Network::HTTP::Request *request) const;

        /**
         *  \brief Prepares a curl easy handle for the request.
         *  \throw Core::Network::Exception if the handle failed to prepare.
         *  \param request the request used to prepare the handle.
         *  \param response the pointer to the response used by curl.
         *  \return the handle.
         */
        curlxx::EasyHandle* prepareHandle(const Core::Network::HTTP::Request *request, Core::Network::HTTP::Response *response) const;

        /**
         *  \brief Prepares a curl easy handle for the request.
         *  \throw Core::Network::Exception if the handle failed to prepare.
         *  \param handle the handle to prepare.
         *  \param request the request used to prepare the handle.
         *  \param response the pointer to the response used by curl.
         */
        void prepareHandle(curlxx::EasyHandle* handle, const Core::Network::HTTP::Request *request, Core::Network::HTTP::Response *response) const;

      protected:
        /**
         *  \brief Used by cURL when reading a response.
         */
        static size_t read_callback(void*, size_t, size_t, void*);

        /**
         *  \brief Used by cURL when sending a request.
         */
        static size_t write_callback(void*, size_t, size_t, void*);

        /**
         *  \brief Used by cURL when reading a header of a response.
         */
        static size_t header_callback(void*, size_t, size_t, void*);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_CONNECTION_HH__ */
