#ifndef   __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__
#define   __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Factory/APooled.hpp"
#include  "Core/Network/Tcp/TcpSocket.hh"

namespace fwk {
    /**
     *  \class TcpSocketStream Core/Network/Tcp/TcpSocketStream.hh
     *  \brief Socket used to receive and send data.
     */
    class TcpSocketStream :public TcpSocket, public APooled<TcpSocketStream> {
    protected:
        ByteArray*  _input; /*!< the input buffer */
        ByteArray*  _output; /*!< the output buffer */

    public:
        static const size_t BUFFER_SIZE; /*!< maximum number of bytes in the input or output buffer before considering it full */

    public:
        /**
         *  \brief Constructor of TcpSocketStream.
         */
        TcpSocketStream(void);

        /**
         *  \brief Destructor of TcpSocketStream.
         */
        virtual ~TcpSocketStream(void);

    public:
        /**
         *  \brief Reinits the TcpSocketStream. Sends back the input and output ByteArrays to their Pool.
         */
        virtual void  reinit(void);

    public:
        /**
         *  \brief Gets I/O ByteArrays from their Pool.
         */
        void  init(void);

        /**
         *  \return true if the output buffer is not empty.
         */
        bool  hasDataToSend(void) const;

        /**
         *  \brief Adds data to the socket.
         *  \param ptr pointer to the start of the data to send.
         *  \param size size in bytes of the data to send.
         *  \throw NetworkException the output buffer is full.
         */
        void  push(const void* ptr, size_t size);

        /**
         *  \brief Reads from the socket and adds it to the input buffer.
         *  \throw NetworkException the input buffer is full or the socket was closed.
         *  \return the number of bytes read.
         */
        ssize_t  recv(void);

        /**
         *  \brief Send data to the socket.
         *  \throw NetworkException the socket was closed.
         *  \return the number of bytes sent.
         */
        ssize_t  send(void);

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

#endif    /* __CORE_NETWORK_TCP_TCPSOCKETSTREAM_HH__ */
