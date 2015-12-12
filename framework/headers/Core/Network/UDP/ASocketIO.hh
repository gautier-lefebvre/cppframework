#ifndef   __CORE_NETWORK_UDP_ASOCKETIO_HH__
#define   __CORE_NETWORK_UDP_ASOCKETIO_HH__

#include  <queue>
#include  <utility>
#include  <netinet/in.h>

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Factory/AFactored.hh"

namespace     Core {
  namespace   Network {
    namespace UDP {
      class   ASocketIO: public virtual Threading::Lockable, public virtual Factory::AFactored {
      protected:
        sockaddr_in _addr;
        std::pair<std::queue<ByteArray*>, size_t> _input;
        std::pair<std::queue<ByteArray*>, size_t> _output;

      public:
        static const size_t BUFFER_SIZE;

      public:
        ASocketIO(void);
        virtual ~ASocketIO(void);

      public:
        virtual void reinit(void);

      public:
        virtual bool hasDataToSend(void) const;
        virtual void push(ByteArray*);
        virtual ByteArray* getData(void);

      public:
        virtual bool operator==(const sockaddr_in&) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_ASOCKETIO_HH__ */
