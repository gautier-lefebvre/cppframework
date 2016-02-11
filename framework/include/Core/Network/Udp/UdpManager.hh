#ifndef   __CORE_NETWORK_UDP_UDPMANAGER_HH__
#define   __CORE_NETWORK_UDP_UDPMANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Library/Factory/APooled.hpp"
#include  "Core/Event/EventHandle.hh"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Network/Udp/UdpSocketServer.hh"
#include  "Core/Network/Udp/UdpSocketClient.hh"
#include  "Core/Network/Udp/UdpSocketStream.hh"

namespace fwk {
  /**
   *  \class UdpSocketStreamEventArgs Core/Network/Udp/UdpManager.hh
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
   *  \class UdpSocketClientEventArgs Core/Network/Udp/UdpManager.hh
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
   *  \class UdpSocketServerEventArgs Core/Network/Udp/UdpManager.hh
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

  /**
   *  \class UdpManager Core/Network/Udp/UdpManager.hh
   *  \brief UDP Manager.
   */
  class UdpManager {
  public:
    /**
     *  \class Server Core/Network/Udp/UdpManager.hh
     *  \brief UDP server and known clients.
     */
    struct Server :public Lockable {
    public:
      uint16_t port; /*!< bound port */
      UdpSocketServer* server; /*!< socket listening on the bound port */
      TLockable<std::list<UdpSocketClient*>> clients; /*!< list of known clients to this server */
      std::set<uint32_t> accept; /*!< accepted IPs */
      std::set<uint32_t> blacklist; /*!< rejected IPs */

      struct Events {
        EventHandle* onNewClient; /*!< Event fired whenever a new client sends a message to this server. Event argument type: UdpSocketClientEventArgs. */
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from a client of this server. Event argument type: UdpSocketClientEventArgs. */
        EventHandle* onClientClosed; /*!< Event fired whenever a known client is removed. Does not work well as UDP is not a connected protocol. Event argument type: UdpSocketClientEventArgs. */
        EventHandle* onClosed; /*!< Event fired when this server is closed. Event argument type: UdpSocketServerEventArgs. */
      } events; /*!< events for this server */

    public:
      /**
       *  \brief Constructor of Server.
       *  \param port bound port.
       *  \param server socket listening to the bound port.
       *  \param accept list of IPs accepted by this server.
       *  \param blacklist list of IPs rejected by this server.
       */
      Server(uint16_t port, UdpSocketServer* server, const std::set<uint32_t>& accept = {}, const std::set<uint32_t>& blacklist = {});

      /**
       *  \brief Destructor of Server.
       */
      virtual ~Server(void);
    };

  public:
    /**
     *  \class Client Core/Network/Udp/UdpManager.hh
     *  \brief UDP client.
     */
    struct Client :public Lockable {
    public:
      std::string hostname; /*!< hostname of the UDP socket this client sends messages to */
      uint16_t port; /*!< port of the UDP socket this client sends messages to */
      UdpSocketStream *socket; /*!< socket */

      struct Events {
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from this socket. Event argument type: UdpSocketStreamEventArgs. */
        EventHandle* onClosed; /*!< Event fired when this socket is closed. Does not work well with UDP protocol. Event argument type: UdpSocketStreamEventArgs. */
      } events; /*!< events for this client */

    public:
      /**
       *  \brief Constructor of Client.
       *  \param hostname the hostname of the UDP socket to send messages to.
       *  \param port the port of the UDP socket to send messages to.
       *  \param socket the socket.
       */
      Client(const std::string& hostname, uint16_t port, UdpSocketStream* socket);

      /**
       *  \brief Destructor of Client.
       */
      virtual ~Client(void);
    };

  public:
    typedef TLockable<std::list<Server>> ServerList;
    typedef TLockable<std::list<Client>> ClientList;

  private:
    ServerList _servers; /*!< bound servers. */
    ClientList _clients; /*!< client sockets. */

    NotifiableThread& _input; /*!< input thread */
    NotifiableThread& _output; /*!< output thread */

  public:
    /**
     *  \brief Constructor of UdpManager.
     *  \param input the input thread.
     *  \param output the output thread.
     */
    UdpManager(NotifiableThread& input, NotifiableThread& output);

    /**
     *  \brief Destructor of UdpManager.
     */
    virtual ~UdpManager(void);

  public:
    /**
     *  \brief Close every server and client.
     */
    void  clear(void);

  public:
    /**
     *  \brief Bind a socket to a specific port.
     *  \throw NetworkException can't bind this port.
     *  \param port the port to bind.
     *  \param accept the IPs to accept. Leave empty to accept any.
     *  \param blacklist the IPs to blacklist.
     *  \return the server.
     */
    const Server&  bind(uint16_t port, const std::set<uint32_t>& accept = {}, const std::set<uint32_t>& blacklist = {});

    /**
     *  \brief Close the socket bound to a specific port and all its clients.
     *  \param port bound port of the socket to close.
     */
    void  close(uint16_t port);

    /**
     *  \brief Close the socket and all its clients.
     *  \param server the server to close.
     */
    void  close(const Server& server);

    /**
     *  \brief Blacklist an IP on a server.
     *  \param port port on which the server is bound.
     *  \param ip the IP to blacklist.
     */
    void  blacklist(uint16_t port, uint32_t ip);

  public:
    /**
     *  \brief connect to a remote UDP server.
     *  \param hostname hostname of the UDP server to connect to.
     *  \param port port of the UDP server to connect to.
     *  \return the client.
     */
    const Client&  connect(const std::string& hostname, uint16_t port);

    /**
     *  \brief Close the socket connected to a specific hostname:port.
     *  \param hostname the hostname the client is connected to.
     *  \param port the port the client is connected to.
     */
    void  close(const std::string& hostname, uint16_t port);

    /**
     *  \brief Close the socket.
     *  \param client the client to close.
     */
    void  close(const Client& client);

  public:
    /**
     *  \brief Push a new datagram to a socket. Closes the socket if its output buffer is full.
     *  \param socket the socket.
     *  \param ptr pointer to the start of the data to send.
     *  \param size the number of bytes of data to send.
     */
    void  push(AUdpSocketIO* socket, const void* ptr, size_t size);

    /**
     *  \brief Push a new datagram to a socket.
     *  \param socket the socket.
     *  \param bytearray the data to send.
     */
    void  push(AUdpSocketIO* socket, const ByteArray* bytearray);

  public:
    /**
     *  \brief Add every UDP socket to the set, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Add every UDP socket to the set if it has data to send, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetWrite(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Send data of every socket in the set.
     *  \param set the set.
     */
    void  send(fd_set& set);

    /**
     *  \brief Read from every socket in the set.
     *  \param set the set.
     */
    void  recv(fd_set&);

  private:
    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle *event, UdpSocketClient *socket, const std::string& message);

    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle *event, UdpSocketStream *socket, const std::string& message);

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, UdpSocketStream *socket) const;

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, UdpSocketClient *socket) const;

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, UdpSocketServer *socket) const;
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPMANAGER_HH__ */
