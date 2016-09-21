#ifndef   __CORE_NETWORK_TCP_TCPSERVER_HH__
#define   __CORE_NETWORK_TCP_TCPSERVER_HH__

#include  <set>
#include  <list>

#include  "Library/Threading/Lockable.hpp"
#include  "Core/Event/EventHandle.hpp"
#include  "Core/Network/Tcp/TcpSocketStream.hh"

namespace fwk {
  /**
   *  \class TcpServer Core/Network/Tcp/TcpServer.hh
   *  \brief TCP server and connected clients.
   */
  struct TcpServer :public Lockable {
  public:
    uint16_t port; /*!< bound port. */
    TcpSocket* server; /*!< socket listening on the bound port. */
    TLockable<std::list<TcpSocketStream*>> clients; /*!< list of connected clients to this server. */
    std::set<uint32_t> accept; /*!< accepted IPs. */
    std::set<uint32_t> blacklist; /*!< rejected IPs. */
    bool active; /*!< the server is running. */

    struct {
      EventHandle<TcpSocketStream*> onAccept; /*!< Event fired whenever a new client connects to this >server. */
      EventHandle<TcpSocketStream*> onReceivedData; /*!< Event fired whenever data is read from a client of this server. */
      EventHandle<TcpSocketStream*> onClientClosed; /*!< Event fired whenever a client of this server closes. */
      EventHandle<TcpSocket*> onClosed; /*!< Event fired when the bound socket is closed. */
    } events; /*!< events for this server */

  public:
    /**
     *  \brief Constructor of TcpServer.
     *  \param port bound port.
     *  \param server socket listening to the bound port.
     */
    TcpServer(uint16_t port, TcpSocket* server);

    /**
     *  \brief Destructor of TcpServer.
     */
    virtual ~TcpServer(void);
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPSERVER_HH__ */
