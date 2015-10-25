#ifndef   __CORE_NETWORK_UDP_SOCKETSTREAM_HH__
#define   __CORE_NETWORK_UDP_SOCKETSTREAM_HH__

#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/UDP/ASocketIO.hh"

namespace     Core {
  namespace   Network {
    namespace UDP {

      /**
       *  Socket "client" (!= client of a server socket)
       */
      class   SocketStream: public ASocketIO, public Factory::HasBasicPool<Core::Network::UDP::SocketStream, 2, 1> {
      private:
        int _fd;
        ByteArray* _buffer;

      public:
        SocketStream(void);
        virtual ~SocketStream(void);

      public:
        virtual void reinit(void);

      public:
        void  init(const std::string&, uint16_t);

      public:
        void  socket(void);
        void  close(void);
        void  sendto(void);
        void  recvfrom(void);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_SOCKETSTREAM_HH__ */
