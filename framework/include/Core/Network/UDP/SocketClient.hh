#ifndef   __CORE_NETWORK_UDP_SOCKETCLIENT_HH__
#define   __CORE_NETWORK_UDP_SOCKETCLIENT_HH__

#include  <sys/select.h>

#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/UDP/ASocketIO.hh"

namespace fwk {
  namespace Core {
    namespace Network {
      namespace UDP {
        /**
         *  \class SocketClient Core/Network/UDP/SocketClient.hh
         *  \brief Client of a server socket. The address of a client who sent a message to a bound UDP server.
         */
        class SocketClient :public ASocketIO, public Factory::TPooled<Core::Network::UDP::SocketClient, 100, 20> {
        private:
          std::pair<uint32_t, uint16_t> _info; /*!< IP and port of the remote client. */

        public:
          /**
           *  \brief Constructor of SocketClient.
           */
          SocketClient(void);

          /**
           *  \brief Destructor of SocketClient.
           */
          virtual ~SocketClient(void);

        public:
          /**
           *  \brief Reinits the IP / port of the socket.
           */
          virtual void  reinit(void);

        public:
          /**
           *  \brief Copies the given address.
           *  \param addr the address to copy.
           */
          void  init(const sockaddr_in& addr);

        public:
          /**
           *  \brief Pops the first datagram of the output queue and returns it. Returns nullptr if the output queue is empty.
           *  \return the next datagram to send.
           */
          ByteArray*  nextDatagram(void);

          /**
           *  \brief Adds a datagram to the input queue. This is meant to be used only by the Core.
           *  \throw Core::Network::Exception the input queue is full.
           *  \param datagram the datagram received.
           */
          void  received(ByteArray* datagram);

        public:
          /**
           *   \return the address structure.
           */
          const sockaddr_in&  socketAddress(void) const;

        public:
          /**
           *  \return the IP / port of the client.
           */
          const std::pair<uint32_t, uint16_t>&  clientInformation(void) const;
        };
      }
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_SOCKETCLIENT_HH__ */
