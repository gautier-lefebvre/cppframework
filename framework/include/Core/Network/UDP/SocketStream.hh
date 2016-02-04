#ifndef   __CORE_NETWORK_UDP_SOCKETSTREAM_HH__
#define   __CORE_NETWORK_UDP_SOCKETSTREAM_HH__

#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/UDP/ASocketIO.hh"
#include  "Core/Network/UDP/ASocket.hh"

namespace fwk {
  namespace Core {
    namespace Network {
      namespace UDP {

        /**
         *  \class SocketStream Core/Network/UDP/SocketStream.hh
         *  \brief A UDP client socket ("connected" to a remote server).
         */
        class SocketStream: public ASocketIO, public ASocket, public Factory::TPooled<Core::Network::UDP::SocketStream, 2, 1> {
        public:
          /**
           *  \brief Constructor of SocketStream.
           */
          SocketStream(void);

          /**
           *  \brief Destructor of SocketStream.
           */
          virtual ~SocketStream(void);

        public:
          /**
           *  \brief See Core::Network::UDP::ASocket::reinit and Core::Network::UDP::ASocketIO::reinit
           */
          virtual void  reinit(void);

        public:
          /**
           *  \brief Sets the hostname and port of the remote server.
           *  \throw Core::Network::Exception gethostbyname failed.
           */
          void  init(const std::string& hostname, uint16_t port);

        public:
          /**
           *  \brief Pops the next datagram and sends it.
           *  \throw Core::Network::Exception sendto failed.
           *  \return the result of sendto.
           */
          ssize_t  sendto(void);

          /**
           *  \brief Receives a datagram and adds it to the input queue.
           *  \throw Core::Network::Exception recvfrom failed, or input buffer full.
           *  \return the result of recvfrom.
           */
          ssize_t  recvfrom(void);
        };
      }
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_SOCKETSTREAM_HH__ */
