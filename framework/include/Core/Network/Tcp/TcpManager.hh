#ifndef   __CORE_NETWORK_TCP_TCPMANAGER_HH__
#define   __CORE_NETWORK_TCP_TCPMANAGER_HH__

#include  <list>

#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Network/Tcp/TcpClient.hh"
#include  "Core/Network/Tcp/TcpServer.hh"
#include  "Core/Network/Tcp/TcpSocket.hh"
#include  "Core/Network/Tcp/TcpSocketStream.hh"

namespace fwk {
  /**
   *  \class TcpManager Core/Network/Tcp/TcpManager.hh
   *  \brief TCP TcpManager.
   */
  class TcpManager {
  public:
    typedef TLockable<std::list<TcpServer>> ServerList; /*!< lockable list of TcpServer. */
    typedef TLockable<std::list<TcpClient>> ClientList; /*!< lockable list of TcpManager:Client. */

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
    const TcpServer&  createServer(uint16_t port);

    /**
     *  \brief Bind the server of the given port and make it listen for clients.
     *  \throw NetworkException can't bind the server port.
     *  \param server the server to launch.
     */
    void  run(const TcpServer& server);

    /**
     *  \brief Close the socket bound to a specific port and all its clients.
     *  \param port bound port of the socket to close.
     */
    void  close(uint16_t port);

    /**
     *  \brief Close the socket and all its clients.
     *  \param server the server to close.
     */
    void  close(const TcpServer& server);

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
    const TcpClient&  createClient(const std::string& hostname, uint16_t port);

    /**
     *  \brief Make the client connect to the server.
     *  \param client the client.
     */
    void  run(const TcpClient& client);

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
    void  close(const TcpClient& client);

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
    void  recv(fd_set& set);

  private:
    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param ss the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle<TcpSocketStream*>* event, TcpSocketStream* ss, const std::string& message);
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPMANAGER_HH__ */
