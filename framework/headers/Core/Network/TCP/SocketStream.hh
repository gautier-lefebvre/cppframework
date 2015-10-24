#ifndef   __CORE_NETWORK_TCP_SOCKETSTREAM_HH__
#define   __CORE_NETWORK_TCP_SOCKETSTREAM_HH__

#include  "Library/Collection/ByteArray.hpp"
#include  "Core/Network/TCP/Socket.hh"

namespace     Core {
  namespace   Network {
    namespace TCP {
      class   SocketStream :public Socket, public Factory::HasBasicPool<Core::Network::TCP::SocketStream, 100, 20> {
      protected:
        ByteArray*  _input;
        ByteArray*  _output;

      public:
        static const size_t BUFFER_SIZE;

      public:
        SocketStream(void);
        virtual ~SocketStream(void);

      public:
        virtual void  reinit(void);

      public:
        void    init(void);
        bool    hasDataToSend(void) const;
        void    push(const void*, size_t);
        ssize_t recv(void);
        ssize_t send(void);

      public:
        size_t  getData(const std::function<size_t (ByteArray&, ByteArray&)>&, ByteArray&);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_TCP_SOCKETSTREAM_HH__ */
