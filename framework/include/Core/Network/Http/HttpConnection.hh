#ifndef   __CORE_NETWORK_HTTP_HTTPCONNECTION_HH__
#define   __CORE_NETWORK_HTTP_HTTPCONNECTION_HH__

#include  <queue>
#include  <thread>

#include  "Library/Network/CURL/EasyHandle.hh"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Network/Http/HttpRequest.hh"
#include  "Core/Network/Http/HttpProtocol.hh"

namespace fwk {
  /**
   *  \class HttpConnection Core/Network/Http/HttpConnection.hh
   *  \brief An open connection to an HTTP server.
   */
  class HttpConnection :public Lockable, public AEndable {
  protected:
    /**
     *  \class upload_object Core/Network/Http/HttpConnection.hh
     *  \brief used by CURL to send requests.
     */
    struct upload_object {
      const uint8_t* ptr; /*!< A pointer to the HttpResponse. */
      size_t length; /*!< Number of bytes read. */
    };

  protected:
    std::string  _host; /*!< hostname of the HTTP server. */
    uint16_t     _port; /*!< port of the HTTP server. */
    HttpProtocol _protocol; /*!< protocol of the server (HTTP/HTTPS/FTP...) */
    std::string  _userAgent; /*!< user agent used for requests. */
    std::thread  *_thread; /*!< thread used by the connection. */
    TNotifiable<std::queue<HttpRequest*>>  _pendingRequests; /*!< pending requests. */

  public:
    /**
     *  \brief Constructor of HttpConnection.
     *  \param hostname hostname of the HTTP server.
     *  \param port port of the HTTP server.
     *  \param protocol protocol used.
     *  \param userAgent user agent used for sending requests.
     */
    HttpConnection(const std::string &hostname, uint16_t port, HttpProtocol protocol, const std::string& userAgent);

    /**
     *  \brief Destructor of HttpConnection. Calls the end method.
     */
    virtual ~HttpConnection(void);

  public:
    /**
     *  \brief Clears pending requests. Wakes any thread waiting on a synchronous request.
     */
    virtual void  onEnd(void);

  public:
    /**
     *  \brief Starts the thread and starts sending pending requests.
     */
    virtual void  run(void);

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
    HttpProtocol  getProtocol(void) const;

    /**
     *  \brief Adds a request to the queue of pending requests.
     *  \param request the request to add.
     */
    void  addRequest(HttpRequest *request);

  protected:
    /**
     *  \brief Send a request. If the request fails, prepares a default 400 response.
     *  \param request the request to send.
     *  \return the response.
     */
    HttpResponse*  sendRequest(const HttpRequest *request) const;

    /**
     *  \brief Send pending requests and waits for new ones until the end method is called.
     */
    void  routine(void);

    /**
     *  \brief Effectively send a request.
     *  \throw NetworkException if the request failed to send.
     *  \param request the request to send.
     *  \return the response.
     */
    HttpResponse*  exec(const HttpRequest *request) const;

    /**
     *  \brief Prepares a curl easy handle for the request.
     *  \throw NetworkException if the handle failed to prepare.
     *  \param request the request used to prepare the handle.
     *  \param response the pointer to the response used by curl.
     *  \return the handle.
     */
    curlxx::EasyHandle*  prepareHandle(const HttpRequest *request, HttpResponse *response) const;

    /**
     *  \brief Prepares a curl easy handle for the request.
     *  \throw NetworkException if the handle failed to prepare.
     *  \param handle the handle to prepare.
     *  \param request the request used to prepare the handle.
     *  \param response the pointer to the response used by curl.
     */
    void  prepareHandle(curlxx::EasyHandle* handle, const HttpRequest *request, HttpResponse *response) const;

    /**
     *  \brief Sets the response to the specified request. Will add the response to the task queue (asynchronous request) or wake the waiting worker (synchronous request).
     *  \param request the request whose response is being set.
     *  \param response the response read. nullptr if the connection is being shut down.
     */
    void  setResponse(HttpRequest* request, HttpResponse* response) const;

  protected:
    /**
     *  \brief Used by cURL when reading a response.
     */
    static size_t  read_callback(void*, size_t, size_t, void*);

    /**
     *  \brief Used by cURL when sending a request.
     */
    static size_t  write_callback(void*, size_t, size_t, void*);

    /**
     *  \brief Used by cURL when reading a header of a response.
     */
    static size_t  header_callback(void*, size_t, size_t, void*);
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPCONNECTION_HH__ */
