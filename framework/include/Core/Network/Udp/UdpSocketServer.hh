#ifndef   __CORE_NETWORK_UDP_UDPSOCKETSERVER_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETSERVER_HH__

#include  <sys/select.h>

#include  "Library/Factory/APooled.hpp"
#include  "Core/Network/Udp/AUdpSocket.hh"
#include  "Core/Network/Udp/UdpSocketClient.hh"

namespace fwk {
  /**
   *  \class UdpSocketServer Core/Network/Udp/UdpSocketServer.hh
   *  \brief A UDP server socket.
   */
  class UdpSocketServer :public AUdpSocket, public APooled<UdpSocketServer>  {
  public:
    /**
     *  \brief Constructor of UdpSocketServer.
     */
    UdpSocketServer(void);

    /**
     *  \brief Destructor of UdpSocketServer.
     */
    virtual ~UdpSocketServer(void);

  public:
    virtual void reinit(void);

  public:
    /**
     *  \brief Binds the specified port.
     *  \throw NetworkException the binding failed.
     *  \param port the port to bind.
     */
    void  bind(uint16_t port);

  public:
    /**
     *  \brief reads on the socket and fills the given address structure with the address of the client.
     *  \throw NetworkException the recvfrom call failed.
     *  \param addr the address structure which will be filled with the address of the client.
     *  \return the datagram received.
     */
    ByteArray*  recvfrom(struct sockaddr_in& addr);

    /**
     *  \brief Pops the next datagram to send from the client socket and sends it.
     *  \param socket the socket of the client.
     *  \return the number of bytes sent.
     */
    ssize_t  sendto(UdpSocketClient* socket);
  };
}

#endif  /* __CORE_NETWORK_UDP_UDPSOCKETSERVER_HH__ */
