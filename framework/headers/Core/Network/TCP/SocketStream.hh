#ifndef   __CORE_NETWORK_TCP_SOCKETSTREAM_HH__
#define   __CORE_NETWORK_TCP_SOCKETSTREAM_HH__

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/TCP/Socket.hh"

namespace     Core {
  namespace   Network {
    namespace TCP {
      class   SocketStream :public Socket, public Factory::TPooled<Core::Network::TCP::SocketStream, 100, 20> {
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
        /**
         *  \brief Gets the first message from the input buffer and removes it.
         *  \param callback a function which takes the input buffer as parameter. This function must return the number of bytes of the first valid message found, or std::npos if not found.
         *  \param dest the buffer into which the message will be stored. It will be resized if necessary.
         */
        size_t  extractData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest);

        /**
         *  \brief Gets the data from the input buffer but does not remove it.
         *  \param callback a function which takes the input buffer as parameter. This function must return the number of bytes of the first valid message found, or std::npos if not found.
         *  \param dest the buffer into which the message will be stored. It will be resized if necessary.
         */
        size_t  getData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest);

        /**
         *  \brief Finds the size of the first valid message in bytes.
         *  \param callback a function which takes the input buffer as parameter. This function must return the number of bytes of the first valid message found, or std::npos if not found.
         *  \returns the result of the callback.
         */
        size_t  seekData(const std::function<size_t (const ByteArray&)>& callback);
      };
    }
  }
}

#endif    /* __CORE_NETWORK_TCP_SOCKETSTREAM_HH__ */
