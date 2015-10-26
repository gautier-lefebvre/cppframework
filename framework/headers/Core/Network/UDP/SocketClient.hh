#ifndef   __CORE_NETWORK_UDP_SOCKETCLIENT_HH__
#define   __CORE_NETWORK_UDP_SOCKETCLIENT_HH__

#include  <sys/select.h>

#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/UDP/ASocketIO.hh"

namespace     Core {
  namespace   Network {
    namespace UDP {

      /**
       *  Client of a server socket
       */
      class   SocketClient :public ASocketIO, public Factory::HasBasicPool<Core::Network::UDP::SocketClient, 100, 20> {
      private:
        std::pair<uint32_t, uint16_t> _info; // IP, port

      public:
        SocketClient(void);
        virtual ~SocketClient(void);

      public:
        virtual void reinit(void);

      public:
        void  init(const sockaddr_in&);

      public:
        ByteArray* nextDatagram(void);
        void  received(ByteArray*);

      public:
        const sockaddr_in&  socketAddress(void) const;

      public:
        const std::pair<uint32_t, uint16_t>& clientInformation(void) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_SOCKETCLIENT_HH__ */
