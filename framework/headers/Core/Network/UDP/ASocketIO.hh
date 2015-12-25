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
      /**
       *  \class ASocketIO Core/Network/UDP/ASocketIO.hh
       *  \brief Superclass of UDP socket who can store input and output datagrams.
       */
      class   ASocketIO: public virtual Threading::Lockable, public virtual Factory::AFactored {
      protected:
        sockaddr_in _addr; /*!< address to read from / write to */
        std::pair<std::queue<ByteArray*>, size_t> _input; /*!< queue of datagrams read / total size of queued datagrams in bytes. */
        std::pair<std::queue<ByteArray*>, size_t> _output; /*!< queue of datagrams to send / total size of queued datagrams in bytes. */

      public:
        static const size_t BUFFER_SIZE; /*!< maximum size of datagrams queues in bytes. */

      public:
        /**
         *  \brief Constructor of ASocketIO.
         */
        ASocketIO(void);

        /**
         *  \brief Destructor of ASocketIO.
         */
        virtual ~ASocketIO(void);

      public:
        /**
         *  \brief Reinits the ASocketIO object. Empties the datagram queues and sets the address to 0 (using memset on the struct).
         */
        virtual void reinit(void);

      public:
        /**
         *  \return true if the output queue is not empty.
         */
        virtual bool hasDataToSend(void) const;

        /**
         *  \brief Adds a datagram to the output queue.
         *  \throw Core::Network::Exception the output queue exceeds the BUFFER_SIZE.
         *  \param datagram the datagram to add.
         */
        virtual void push(ByteArray* datagram);

        /**
         *  \brief Pops the first datagram of the input queue and returns it. Returns nullptr if the input queue is empty.
         *  \return the first datagram read on this socket, or nullptr if empty.
         */
        virtual ByteArray* getData(void);

      public:
        /**
         *  \return true if the address is the same as this socket. Compares the protocol, IP and port.
         *  \param addr the address to compare.
         */
        virtual bool operator==(const sockaddr_in& addr) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_ASOCKETIO_HH__ */
