#ifndef   __CORE_NETWORK_UDP_SOCKETSERVER_HH__
#define   __CORE_NETWORK_UDP_SOCKETSERVER_HH__

#include  <sys/select.h>

#include  "Library/Threading/Lock.hpp"
#include  "Library/Factory/IFactored.hh"
#include  "Core/Network/UDP/SocketClient.hh"

namespace     Core {
  namespace   Network {
    namespace UDP {

      /**
       *  Server socket
       */
      class SocketServer :public Factory::IFactored, public Threading::Lock {
      private:
        int _fd;
        ByteArray* _buffer;

      public:
        SocketServer(void);
        virtual ~SocketServer(void);

      public:
        virtual void reinit(void);

      public:
        void  init(void);
        void  socket(void);
        void  close(void);
        void  bind(uint16_t);

      public:
        ByteArray* recvfrom(struct sockaddr&);
        ssize_t  sendto(SocketClient*);

      public:
        void  addToSet(fd_set&, int&) const;
        bool  isset(fd_set&) const;
      };
    }
  }
}

#endif  /* __CORE_NETWORK_UDP_SOCKETSERVER_HH__ */
