#ifndef   __CORE_NETWORK_TCP_TCPEVENTS_HH__
#define   __CORE_NETWORK_TCP_TCPEVENTS_HH__

#include  "Library/Factory/APooled.hpp"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Network/Tcp/TcpSocket.hh"
#include  "Core/Network/Tcp/TcpSocketStream.hh"

namespace fwk {
  /**
   *  \class TcpSocketStreamEventArgs Core/Network/Tcp/TcpEvents.hh
   *  \brief Used when firing an event sending a TcpSocketStream as argument.
   */
  struct TcpSocketStreamEventArgs :public IEventArgs, public APooled<TcpSocketStreamEventArgs> {
  public:
    TcpSocketStream* socket; /*!< the SocketStream object. */

  public:
    /**
     *  \brief Constructor of TcpSocketStreamEventArgs.
     */
    TcpSocketStreamEventArgs(void);

  public:
    /**
     *  \brief Reinits the TcpSocketStreamEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the SocketStream to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the SocketStream.
     *  \param ss the SocketStream.
     */
    void  init(TcpSocketStream* ss);
  };

  /**
   *  \class TcpSocketEventArgs Core/Network/Tcp/TcpEvents.hh
   *  \brief Used when firing an event sending a TcpSocket as argument.
   */
  struct TcpSocketEventArgs :public IEventArgs, public APooled<TcpSocketEventArgs> {
  public:
    TcpSocket* socket; /*!< the Socket object. */

  public:
    /**
     *  \brief Constructor of TcpSocketEventArgs.
     */
    TcpSocketEventArgs(void);

  public:
    /**
     *  \brief Reinits the TcpSocketEventArgs object.
     */
    virtual void  reinit(void);

    /**
     *  \brief Sends the Socket to its pool. Done automatically after the event has been fired.
     */
    virtual void  cleanup(void);

    /**
     *  \brief Sets the Socket.
     *  \param s the Socket.
     */
    void  init(TcpSocket* s);
  };
}

#endif  /* __CORE_NETWORK_TCP_TCPEVENTS_HH__ */
