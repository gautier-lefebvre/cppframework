#ifndef   __CORE_NETWORK_UDP_UDPSERVER_HH__
#define   __CORE_NETWORK_UDP_UDPSERVER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lockable.hpp"
#include  "Core/Event/EventHandle.hpp"
#include  "Core/Network/Udp/UdpSocketClient.hh"
#include  "Core/Network/Udp/UdpSocketServer.hh"

namespace fwk {
    /**
     *  \class UdpServer Core/Network/Udp/UdpServer.hh
     *  \brief UDP server and known clients.
     */
    struct UdpServer :public Lockable {
    public:
        uint16_t port; /*!< bound port */
        UdpSocketServer* server; /*!< socket listening on the bound port */
        TLockable<std::list<UdpSocketClient*>> clients; /*!< list of known clients to this server */
        std::set<uint32_t> accept; /*!< accepted IPs */
        std::set<uint32_t> blacklist; /*!< rejected IPs */
        bool active; /*!< the server is running. */

        struct {
            EventHandle<UdpSocketClient*> onNewClient; /*!< Event fired whenever a new client sends a message to this server. */
            EventHandle<UdpSocketClient*> onReceivedData; /*!< Event fired whenever data is read from a client of this server. */
            EventHandle<UdpSocketClient*> onClientClosed; /*!< Event fired whenever a known client is removed. Does not work well as UDP is not a connected protocol. */
            EventHandle<UdpSocketServer*> onClosed; /*!< Event fired when this server is closed. */
        } events; /*!< events for this server */

    public:
        /**
         *  \brief Constructor of UdpServer.
         *  \param port bound port.
         *  \param server socket listening to the bound port.
         */
        UdpServer(uint16_t port, UdpSocketServer* server);

        /**
         *  \brief Destructor of UdpServer.
         */
        virtual ~UdpServer(void);
    };
}

#endif    /* __CORE_NETWORK_UDP_UDPSERVER_HH__ */
