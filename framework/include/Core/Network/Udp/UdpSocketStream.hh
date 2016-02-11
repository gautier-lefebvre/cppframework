#ifndef   __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__
#define   __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__

#include  "Library/Factory/APooled.hpp"
#include  "Core/Network/Udp/AUdpSocketIO.hh"
#include  "Core/Network/Udp/AUdpSocket.hh"

namespace fwk {
  /**
   *  \class UdpSocketStream Core/Network/UDP/UdpSocketStream.hh
   *  \brief A UDP client socket ("connected" to a remote server).
   */
  class UdpSocketStream: public AUdpSocketIO, public AUdpSocket, public APooled<UdpSocketStream> {
  public:
    /**
     *  \brief Constructor of UdpSocketStream.
     */
    UdpSocketStream(void);

    /**
     *  \brief Destructor of UdpSocketStream.
     */
    virtual ~UdpSocketStream(void);

  public:
    /**
     *  \brief See AUdpSocket::reinit and AUdpSocketIO::reinit
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the hostname and port of the remote server.
     *  \throw NetworkException gethostbyname failed.
     */
    void  init(const std::string& hostname, uint16_t port);

  public:
    /**
     *  \brief Pops the next datagram and sends it.
     *  \throw NetworkException sendto failed.
     *  \return the result of sendto.
     */
    ssize_t  sendto(void);

    /**
     *  \brief Receives a datagram and adds it to the input queue.
     *  \throw NetworkException recvfrom failed, or input buffer full.
     *  \return the result of recvfrom.
     */
    ssize_t  recvfrom(void);
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPSOCKETSTREAM_HH__ */
