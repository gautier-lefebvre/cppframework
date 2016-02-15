#ifndef   __CORE_NETWORK_UDP_UDPEVENTS_HH__
#define   __CORE_NETWORK_UDP_UDPEVENTS_HH__

#include  "Library/Factory/APooled.hpp"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Network/Udp/UdpSocketClient.hh"
#include  "Core/Network/Udp/UdpSocketServer.hh"
#include  "Core/Network/Udp/UdpSocketStream.hh"

namespace fwk {
    /**
   *  \class UdpSocketStreamEventArgs Core/Network/Udp/UdpEvents.hh
   *  \brief Used when firing an event sending a UdpSocketStream as argument.
   */
  struct UdpSocketStreamEventArgs :public IEventArgs, public APooled<UdpSocketStreamEventArgs> {
  public:
    UdpSocketStream* socket; /*!< the SocketStream object. */

  public:
    /**
     *  \brief Constructor of UdpSocketStreamEventArgs.
     */
    UdpSocketStreamEventArgs(void);

  public:
    /**
     *  \brief Reinits the UdpSocketStreamEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketStream to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketStream.
     *  \param socket the SocketStream.
     */
    virtual void  init(UdpSocketStream* socket);
  };

  /**
   *  \class UdpSocketClientEventArgs Core/Network/Udp/UdpEvents.hh
   *  \brief Used when firing an event sending a UdpSocketClient as argument.
   */
  struct UdpSocketClientEventArgs :public IEventArgs, public APooled<UdpSocketClientEventArgs> {
  public:
    UdpSocketClient* socket; /*!< the SocketClient object. */

  public:
    /**
     *  \brief Constructor of UdpSocketClientEventArgs.
     */
    UdpSocketClientEventArgs(void);

  public:
    /**
     *  \brief Reinits the UdpSocketClientEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketClient to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketClient.
     *  \param socket the SocketClient.
     */
    virtual void  init(UdpSocketClient* socket);
  };

  /**
   *  \class UdpSocketServerEventArgs Core/Network/Udp/UdpEvents.hh
   *  \brief Used when firing an event sending a UdpSocketServer as argument.
   */
  struct UdpSocketServerEventArgs :public IEventArgs, public APooled<UdpSocketServerEventArgs> {
  public:
    UdpSocketServer* socket; /*!< the SocketServer object. */

  public:
    /**
     *  \brief Constructor of UdpSocketServerEventArgs.
     */
    UdpSocketServerEventArgs(void);

  public:
    /**
     *  \brief Reinits the UdpSocketServerEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketServer to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketServer.
     *  \param socket the SocketServer.
     */
    virtual void  init(UdpSocketServer* socket);
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPEVENTS_HH__ */
