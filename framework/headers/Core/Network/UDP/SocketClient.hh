#ifndef   __CORE_NETWORK_UDP_SOCKETCLIENT_HH__
#define   __CORE_NETWORK_UDP_SOCKETCLIENT_HH__

#include  <sys/select.h>
#include  <netinet/in.h>
#include  <queue>
#include  <utility>

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Factory/AFactored.hh"

namespace     Core {
  namespace   Network {
    namespace UDP {
      class   SocketClient :public Factory::AFactored, public Threading::Lock, public Factory::HasBasicPool<Core::Network::UDP::SocketClient, 100, 20> {
      private:
        sockaddr_in _addr;
        std::pair<uint32_t, uint16_t> _info; // IP, port
        std::pair<std::queue<ByteArray*>, size_t> _input;
        std::pair<std::queue<ByteArray*>, size_t> _output;

        static const size_t BUFFER_SIZE = 32768;

      public:
        SocketClient(void);
        virtual ~SocketClient(void);

      public:
        virtual void reinit(void);

      public:
        void  init(const sockaddr_in&);

      public:
        bool  hasDataToSend(void) const;
        void  push(ByteArray*);

        ByteArray*  nextDatagram(void);
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
