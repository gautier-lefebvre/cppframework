#ifndef   __CORE_NETWORK_TCP_TCPMANAGER_HH__
#define   __CORE_NETWORK_TCP_TCPMANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Library/Factory/APooled.hpp"
#include  "Core/Network/Tcp/TcpSocket.hh"
#include  "Core/Network/Tcp/TcpSocketStream.hh"
#include  "Core/Event/EventHandle.hh"
#include  "Core/Event/IEventArgs.hh"

namespace fwk {
  /**
   *  \class TcpSocketStreamEventArgs Core/Network/Tcp/Manager.hh
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
   *  \class TcpSocketEventArgs Core/Network/Tcp/Manager.hh
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

  /**
   *  \class TcpManager Core/Network/Tcp/TcpManager.hh
   *  \brief TCP TcpManager.
   */
  class TcpManager {
  public:
    /**
     *  \class Server Core/Network/Tcp/TcpManager.hh
     *  \brief TCP server and connected clients.
     */
    struct Server :public Lockable {
    public:
      uint16_t port; /*!< bound port. */
      TcpSocket* server; /*!< socket listening on the bound port. */
      TLockable<std::list<TcpSocketStream*>> clients; /*!< list of connected clients to this server. */
      std::set<uint32_t> accept; /*!< accepted IPs. */
      std::set<uint32_t> blacklist; /*!< rejected IPs. */
      bool active; /*!< the server is running. */

      struct Events {
        EventHandle* onAccept; /*!< Event fired whenever a new client connects to this server. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from a client of this server. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onClientClosed; /*!< Event fired whenever a client of this server closes. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onClosed; /*!< Event fired when the bound socket is closed. Event argument type: TcpSocketEventArgs. */
      } events; /*!< events for this server */

    public:
      /**
       *  \brief Constructor of Server.
       *  \param port bound port.
       *  \param server socket listening to the bound port.
       *  \param accept list of IPs accepted by this server.
       *  \param blacklist list of IPs rejected by this server.
       */
      Server(uint16_t port, TcpSocket* server);

      /**
       *  \brief Destructor of Server.
       */
      virtual ~Server(void);
    };

  public:
    /**
     *  \class Client Core/Network/Tcp/Manager.hh
     *  \brief TCP client.
     */
    struct Client :public Lockable {
    public:
      std::string hostname; /*!< hostname of the TCP socket this client is connected to. */
      uint16_t port; /*!< port of the TCP socket this client is connected to. */
      TcpSocketStream *socket; /*!< connected socket. */
      bool active; /*!< the client is running. */

      struct Events {
        EventHandle* onReceivedData; /*!< Event fired whenever data is read from this socket. Event argument type: TcpSocketStreamEventArgs. */
        EventHandle* onClosed; /*!< Event fired when this socket is closed. Event argument type: TcpSocketStreamEventArgs. */
      } events; /*!< events for this client */

    public:
      /**
       *  \brief Constructor of Client.
       *  \param hostname the hostname of the TCP socket.
       *  \param port the port of the TCP socket.
       *  \param socket the connected socket.
       */
      Client(const std::string& hostname, uint16_t port, TcpSocketStream* socket);

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
    ClientList _clients; /*!< connected clients. */

    NotifiableThread& _input; /*!< input thread */
    NotifiableThread& _output; /*!< output thread */

  public:
    /**
     *  \brief Constructor of TcpManager.
     *  \param input the input thread.
     *  \param output the output thread.
     */
    TcpManager(NotifiableThread& input, NotifiableThread& output);

    /**
     *  \brief Destructor of TcpManager.
     */
    virtual ~TcpManager(void);

  public:
    /**
     *  \brief Close every server and client.
     */
    void  clear(void);

  public:
    /**
     *  \brief Create a new server.
     *  \param port the port to bind (does not do the binding).
     *  \return the server.
     */
    const Server&  createServer(uint16_t port);

    /**
     *  \brief Bind the server of the given port and make it listen for clients.
     *  \throw NetworkException can't bind the server port.
     *  \param server the server to launch.
     */
    void  run(const Server& server);

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
     *  \brief Create a connection to a remote TCP server.
     *  \param hostname hostname of the TCP server to connect to.
     *  \param port port of the TCP server to connect to.
     *  \return the client.
     */
    const Client&  createClient(const std::string& hostname, uint16_t port);

    /**
     *  \brief Make the client connect to the server.
     *  \param client the client.
     */
    void  run(const Client& client);

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
     *  \brief Push data to a socket. Closes the socket if its output buffer is full.
     *  \param ss the socket.
     *  \param ptr pointer to the start of the data to send.
     *  \param size the number of bytes of data to send.
     */
    void  push(TcpSocketStream* ss, const void* ptr, size_t size);

    /**
     *  \brief Push data to a socket.
     *  \param ss the socket.
     *  \param bytearray the data to send.
     */
    void  push(TcpSocketStream* ss, const ByteArray* bytearray);

  public:
    /**
     *  \brief Add every TCP socket to the set, increments the number of sockets add, and updates the max fd.
     *  \param set the set.
     *  \param max_fd the max fd number.
     *  \param nb the number of sockets added to the set.
     */
    void  fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);

    /**
     *  \brief Add every TCP socket to the set if it has data to send, increments the number of sockets add, and updates the max fd.
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
     *  \param ss the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle* event, TcpSocketStream* ss, const std::string& message);

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, TcpSocketStream* socket) const;

    /**
     *  \brief Fires an event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     */
    void  __fireEvent(EventHandle *event, TcpSocket* socket) const;
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPMANAGER_HH__ */
