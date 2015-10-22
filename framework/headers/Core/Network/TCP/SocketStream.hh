#ifndef   __CORE_NETWORK_TCP_SOCKETSTREAM_HH__
#define   __CORE_NETWORK_TCP_SOCKETSTREAM_HH__

#include  "Library/Collection/ByteArray.hpp"
#include  "Core/Network/TCP/Socket.hh"

namespace     Core {
  namespace   Network {
    namespace TCP {
      class   SocketStream :public Socket {
      protected:
        ByteArray*  _input;
        ByteArray*  _output;

      public:
        static const size_t BUFFER_SIZE = 32768;

      public:
        SocketStream(void);
        virtual ~SocketStream(void);

      public:
        virtual void  reinit(void);

      public:
        void  init(void);
        bool  hasDataToSend(void) const;
        void  push(const void*, size_t);
        void  recv(void);
        void  send(void);

      public:
        size_t  getData(const std::function<size_t (ByteArray&, ByteArray&)>&, ByteArray&);

      public:
        static Factory::Pool<Core::Network::TCP::SocketStream> *Pool;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_TCP_SOCKETSTREAM_HH__ */
