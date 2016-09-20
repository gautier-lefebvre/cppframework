#ifndef   __CORE_NETWORK_UDP_UDPCLIENT_HH__
#define   __CORE_NETWORK_UDP_UDPCLIENT_HH__

#include  "Library/Threading/Lockable.hpp"
#include  "Core/Event/EventHandle.hpp"
#include  "Core/Network/Udp/UdpSocketStream.hh"

namespace fwk {
  /**
   *  \class UdpClient Core/Network/Udp/UdpClient.hh
   *  \brief UDP client.
   */
  struct UdpClient :public Lockable {
  public:
    std::string hostname; /*!< hostname of the UDP socket this client sends messages to */
    uint16_t port; /*!< port of the UDP socket this client sends messages to */
    UdpSocketStream *socket; /*!< socket */
    bool active; /*!< the client is running. */

    struct {
      EventHandle<UdpSocketStream*>* onReceivedData; /*!< Event fired whenever data is read from this socket. */
      EventHandle<UdpSocketStream*>* onClosed; /*!< Event fired when this socket is closed. Does not work well with UDP protocol. */
    } events; /*!< events for this client */

  public:
    /**
     *  \brief Constructor of UdpClient.
     *  \param hostname the hostname of the UDP socket to send messages to.
     *  \param port the port of the UDP socket to send messages to.
     *  \param socket the socket.
     */
    UdpClient(const std::string& hostname, uint16_t port, UdpSocketStream* socket);

    /**
     *  \brief Destructor of UdpClient.
     */
    virtual ~UdpClient(void);
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPCLIENT_HH__ */
