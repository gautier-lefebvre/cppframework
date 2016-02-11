#ifndef    __CORE_NETWORK_NETWORKMANAGER_HH__
#define    __CORE_NETWORK_NETWORKMANAGER_HH__

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Property/Initializable.hpp"
#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Network/Tcp/TcpManager.hh"
#include  "Core/Network/Udp/UdpManager.hh"

namespace fwk {
  class NetworkManager :public Singleton<fwk::NetworkManager>, public Lockable, public AEndable, public Initializable {
    friend class Singleton<fwk::NetworkManager>;
  public:
    NotifiableThread _input; /*!< the input thread. */
    NotifiableThread _output; /*!< the output thread. */

    TcpManager _tcp; /*!< the TCP network manager. */
    UdpManager _udp; /*!<  the UDP network manager.*/

  private:
    /**
     *  \brief Deleted copy constructor of NetworkManager.
     */
    NetworkManager(const NetworkManager&) = delete;

    /**
     *  \brief Deleted copy constructor of NetworkManager.
     */
    NetworkManager(const NetworkManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of NetworkManager.
     */
    NetworkManager&  operator=(const NetworkManager&) = delete;

  private:
    /**
     *  \brief Constructor of NetworkManager.
     */
    NetworkManager(void);

    /**
     *  \brief Destructor of NetworkManager.
     */
    virtual ~NetworkManager(void);

  public:
    /**
     *  \brief Clears the I/O threads of TCP and UDP networks, closing any server and client.
     */
    virtual void  end(void);

  public:
    /**
     *  \brief Creates the I/O threads the first time it is called.
     */
    void  init(void);

    /**
     *  \brief Clears the TCP network, closing any server and client.
     */
    void  endTCP(void);

    /**
     *  \brief Clears the UDP network, closing any server and client.
     */
    void  endUDP(void);

  private:
    /**
     *  \brief Until the end method is called, reads on the UDP/TCP sockets.
     */
    void  inputRoutine(void);

    /**
     *  \brief Until the end method is called, writes on the UDP/TCP sockets.
     */
    void  outputRoutine(void);

  public:
    /**
     *  \return the TCP manager.
     */
    TcpManager&  getTCP(void);

    /**
     *  \return the UDP manager.
     */
    UdpManager&  getUDP(void);
  };
}

#endif    /* __CORE_NETWORK_NETWORKMANAGER_HH__ */
