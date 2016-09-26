#ifndef   __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__
#define   __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__

#include  <string>

#include  "Library/Collection/BidiMap.hpp"

namespace fwk {
    /**
     *  \enum HttpProtocol Core/Network/Http/HttpProtocol.hh
     *  \brief protocols used in the http client.
     */
    enum class HttpProtocol {
        HTTP,
        HTTPS,
        FTP
    };

    static const BidiMap<HttpProtocol, const std::string> HttpProtocolToString = {
        {HttpProtocol::HTTP, std::string("http")},
        {HttpProtocol::HTTPS, std::string("https")},
        {HttpProtocol::FTP, std::string("ftp")},
    }; /*!< Used to translate the enum Logging::Level to a string. */
}

#endif    /* __CORE_NETWORK_HTTP_HTTPPROTOCOL_HH__ */
