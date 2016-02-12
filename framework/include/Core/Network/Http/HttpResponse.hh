#ifndef   __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__
#define   __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__

#include  "Library/Factory/APooled.hpp"
#include  "Core/Network/Http/AHttpMessage.hh"

namespace fwk {
  /**
   *  \class HttpResponse Core/Network/Http/HttpResponse.hh
   *  \brief a HttpResponse received from a HTTP server.
   */
  struct HttpResponse :public AHttpMessage, public APooled<HttpResponse> {
    /**
     *  \enum Status Core/Network/Http/HttpResponse.hh
     *  \brief Meaning of HTTP status.
     */
    enum class Status {
      CONTINUE                        = 100,
      SWITCHING_PROTOCOLS             = 101,
      PROCESSING                      = 102,

      OK                              = 200,
      CREATED                         = 201,
      ACCEPTED                        = 202,
      NON_AUTHORITATIVE_INFORMATION   = 203,
      NO_CONTENT                      = 204,
      RESET_CONTENT                   = 205,
      PARTIAL_CONTENT                 = 206,
      MULTI_STATUS                    = 207,
      IM_USED                         = 226,

      MULTIPLE_CHOICES                = 300,
      MOVED_PERMANENTLY               = 301,
      FOUND                           = 302,
      SEE_OTHER                       = 303,
      NOT_MODIFIED                    = 304,
      USE_PROXY                       = 305,
      TEMPORARY_REDIRECT              = 307,

      BAD_REQUEST                     = 400,
      UNAUTHORIZED                    = 401,
      PAYMENT_REQUIRED                = 402,
      FORBIDDEN                       = 403,
      NOT_FOUND                       = 404,
      METHOD_NOT_ALLOWED              = 405,
      NOT_ACCEPTABLE                  = 406,
      PROXY_AUTHENTICATION_REQUIRED   = 407,
      REQUEST_TIMEOUT                 = 408,
      CONFLICT                        = 409,
      GONE                            = 410,
      LENGTH_REQUIRED                 = 411,
      PRECONDITION_FAILED             = 412,
      REQUEST_ENTITY_TOO_LARGE        = 413,
      REQUEST_URI_TOO_LONG            = 414,
      UNSUPPORTED_MEDIA_TYPE          = 415,
      REQUESTED_RANGE_NOT_SATISFIABLE = 416,
      EXPECTATION_FAILED              = 417,
      UNPROCESSABLE_ENTITY            = 422,
      LOCKED                          = 423,
      FAILED_DEPENDENCY               = 424,
      UPGRADE_REQUIRED                = 426,

      INTERNAL_SERVER_ERROR           = 500,
      NOT_IMPLEMENTED                 = 501,
      BAD_GATEWAY                     = 502,
      SERVICE_UNAVAILABLE             = 503,
      GATEWAY_TIMEOUT                 = 504,
      HTTP_VERSION_NOT_SUPPORTED      = 505,
      INSUFFICIENT_STORAGE            = 507,
      NOT_EXTENDED                    = 510
    };

    uint32_t  status; /*!< status of the response (200, 400, ...). */
    std::string reason; /*!< reason of the response ("Success", "Bad Request", ...). This does not work with cURL. */

    /**
     *  \brief Constructor of HttpResponse.
     */
    HttpResponse(void);

    /**
     *  \brief Destructor of HttpResponse.
     */
    virtual ~HttpResponse(void);

    /**
     *  \brief Reinits the HttpResponse.
     */
    virtual void  reinit(void);
  };
}

#endif    /* __CORE_NETWORK_HTTP_HTTPRESPONSE_HH__ */
