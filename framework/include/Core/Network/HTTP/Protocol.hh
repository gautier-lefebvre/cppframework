#ifndef   __CORE_NETWORK_HTTP_PROTOCOL_HH__
#define   __CORE_NETWORK_HTTP_PROTOCOL_HH__

#include  <string>

#include  "Library/Collection/BidiMap.hpp"

namespace Core {
  namespace Network {
    namespace HTTP {
      /**
       *  \class Protocol Core/Network/HTTP/Protocol.hh
       *  \brief protocols used in the http client.
       */
      enum class Protocol {
        HTTP,
        HTTPS,
        FTP
      };

      static const BidiMap<Protocol, const std::string> ProtocolToString = {
        {Protocol::HTTP, std::string("http")},
        {Protocol::HTTPS, std::string("https")},
        {Protocol::FTP, std::string("ftp")},
      }; /*!< Used to translate the enum Logging::Level to a string. */
    }
  }
}

#endif    /* __CORE_NETWORK_HTTP_PROTOCOL_HH__ */
