#ifndef   __CORE_NETWORK_UDP_ASOCKET_HH__
#define   __CORE_NETWORK_UDP_ASOCKET_HH__

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Factory/AFactored.hh"
#include  "Library/Threading/Lock.hpp"

namespace     Core {
  namespace   Network {
    namespace UDP {
      class   ASocket :public virtual Threading::Lockable, public virtual Factory::AFactored {
      protected:
        int _fd;
        ByteArray* _buffer;

      public:
        ASocket(void);
        virtual ~ASocket(void);

      public:
        virtual void reinit(void);

      public:
        virtual void  socket(void);
        virtual void  close(void);

      public:
        virtual void  addToSet(fd_set&, int&) const;
        virtual bool  isset(fd_set&) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_ASOCKET_HH__ */
