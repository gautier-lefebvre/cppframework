#ifndef   __CORE_NETWORK_UDP_SOCKETSERVER_HH__
#define   __CORE_NETWORK_UDP_SOCKETSERVER_HH__

#include  <sys/select.h>

#include  "Core/Network/UDP/ASocket.hh"
#include  "Core/Network/UDP/SocketClient.hh"

namespace fwk {
  namespace Core {
    namespace Network {
      namespace UDP {
        /**
         *  \class SocketServer Core/Network/UDP/SocketServer.hh
         *  \brief A UDP server socket.
         */
        class SocketServer :public ASocket, public Factory::TPooled<Core::Network::UDP::SocketServer, 2, 1>  {
        public:
          /**
           *  \brief Constructor of SocketServer.
           */
          SocketServer(void);

          /**
           *  \brief Destructor of SocketServer.
           */
          virtual ~SocketServer(void);

        public:
          /**
           *  \brief Binds the specified port.
           *  \throw Core::Network::Exception the binding failed.
           *  \param port the port to bind.
           */
          void  bind(uint16_t port);

        public:
          /**
           *  \brief reads on the socket and fills the given address structure with the address of the client.
           *  \throw Core::Network::Exception the recvfrom call failed.
           *  \param addr the address structure which will be filled with the address of the client.
           *  \return the datagram received.
           */
          ByteArray*  recvfrom(struct sockaddr_in& addr);

          /**
           *  \brief Pops the next datagram to send from the client socket and sends it.
           *  \param socket the socket of the client.
           *  \return the number of bytes sent.
           */
          ssize_t  sendto(SocketClient* socket);
        };
      }
    }
  }
}

#endif  /* __CORE_NETWORK_UDP_SOCKETSERVER_HH__ */
