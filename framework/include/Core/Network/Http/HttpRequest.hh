#ifndef   __CORE_NETWORK_HTTP_HTTPREQUEST_HH__
#define   __CORE_NETWORK_HTTP_HTTPREQUEST_HH__

#include  <functional>

#include  "Library/Factory/APooled.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Network/Http/AHttpMessage.hh"
#include  "Core/Network/Http/HttpResponse.hh"

namespace fwk {
    /**
     *  \class HttpRequest Core/Network/Http/HttpRequest.hh
     *  \brief A request to send to a HTTP server.
     */
    struct HttpRequest :public AHttpMessage, public APooled<HttpRequest> {
        std::string method; /*!< method (or verb) of the request. */
        std::string url; /*!< url of the request (ex: "/posts"). */
        std::function<void (const HttpResponse *)> success; /*!< callback called when the response has a status < 400. */
        std::function<void (const HttpResponse *)> error; /*!< callback called when the response >= 400. */
        std::function<void (void)> clean; /*!< callback called when the response is not valid. */

        struct {
            bool isFile; /*!< wether of not the request must upload a file. */
            std::string filepath; /*!< path of the file to upload. */
        } file; /*!< information on a file to send. */

        struct {
            bool isAsynchronous; /*!< wether or not the request is synchronous. The other attributes of this structure are invalid if this variable is true. */
            bool isValid; /*!< if false, the thread was only woken because the connection is being closed. */
            Notifiable lock; /*!< Notifiable variable to wait on until the response is read. */
            HttpResponse* response; /*!< the response to the request. This is only set in case of a synchronous request. */
        } asynchronous; /*!< information on a synchronous request. */

        /**
         *  \brief Constructor of HttpRequest.
         */
        HttpRequest(void);

        /**
         *  \brief Destructor of HttpRequest.
         */
        virtual ~HttpRequest(void);

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
         *  \return true if the request received a response. false if the Notifiable variable was notified because the connection is being closed.
         */
        bool  wait(void);

        /**
         *  \brief Wakes the synchronous request.
         *  \param response the http response read. If nullptr, means that the response was not read and the thread was only woken because the client is being shut down.
         */
        void  wake(HttpResponse* response);

        /**
            synchronous request usage:

            HttpRequest* request = HttpRequest::getFromPool();
            request->method = ...;
            request->url = ...;
            ...

            request->asynchronous.isAsynchronous = false;

            HttpClient::get().sendRequest(request, hostname, [port, secureport]);

            if (request->wait()) { handle response}
            else { clean (no response) }

            HttpRequest::returnToPool(request);
        */
    };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPREQUEST_HH__ */
