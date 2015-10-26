#ifndef   __CORE_NETWORK_UDP_SOCKETSERVER_HH__
#define   __CORE_NETWORK_UDP_SOCKETSERVER_HH__

#include  <sys/select.h>

#include  "Core/Network/UDP/ASocket.hh"
#include  "Core/Network/UDP/SocketClient.hh"

namespace     Core {
  namespace   Network {
    namespace UDP {

      /**
       *  Server socket
       */
      class SocketServer :public ASocket, public Factory::HasBasicPool<Core::Network::UDP::SocketServer, 2, 1>  {
      public:
        SocketServer(void);
        virtual ~SocketServer(void);

      public:
        void  init(void);
        void  bind(uint16_t);

      public:
        ByteArray* recvfrom(struct sockaddr_in&);
        ssize_t  sendto(SocketClient*);
      };
    }
  }
}

#endif  /* __CORE_NETWORK_UDP_SOCKETSERVER_HH__ */
