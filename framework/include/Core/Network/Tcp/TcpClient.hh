#ifndef   __CORE_NETWORK_TCP_TCPCLIENT_HH__
#define   __CORE_NETWORK_TCP_TCPCLIENT_HH__

#include  <string>

#include  "Library/Threading/Lockable.hpp"
#include  "Core/Event/EventHandle.hpp"
#include  "Core/Network/Tcp/TcpSocketStream.hh"

namespace fwk {
    /**
     *  \class TcpClient Core/Network/Tcp/TcpClient.hh
     *  \brief TCP client.
     */
    struct TcpClient :public Lockable {
    public:
        std::string hostname; /*!< hostname of the TCP socket this client is connected to. */
        uint16_t port; /*!< port of the TCP socket this client is connected to. */
        TcpSocketStream *socket; /*!< connected socket. */
        bool active; /*!< the client is running. */

        struct {
            EventHandle<TcpSocketStream*> onReceivedData; /*!< Event fired whenever data is read from this socket. */
            EventHandle<TcpSocketStream*> onClosed; /*!< Event fired when this socket is closed. */
        } events; /*!< events for this client */

    public:
        /**
         *  \brief Constructor of TcpClient.
         *  \param hostname the hostname of the TCP socket.
         *  \param port the port of the TCP socket.
         *  \param socket the connected socket.
         */
        TcpClient(const std::string& hostname, uint16_t port, TcpSocketStream* socket);

        /**
         *  \brief Destructor of TcpClient.
         */
        virtual ~TcpClient(void);
    };
}

#endif    /* __CORE_NETWORK_TCP_TCPCLIENT_HH__ */
