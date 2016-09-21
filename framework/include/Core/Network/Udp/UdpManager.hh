#ifndef   __CORE_NETWORK_UDP_UDPMANAGER_HH__
#define   __CORE_NETWORK_UDP_UDPMANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Event/EventHandle.hpp"
#include  "Core/Network/Udp/UdpClient.hh"
#include  "Core/Network/Udp/UdpServer.hh"
#include  "Core/Network/Udp/UdpSocketServer.hh"
#include  "Core/Network/Udp/UdpSocketClient.hh"
#include  "Core/Network/Udp/UdpSocketStream.hh"

namespace fwk {
  /**
   *  \class UdpManager Core/Network/Udp/UdpManager.hh
   *  \brief UDP Manager.
   */
  class UdpManager {
  public:
    typedef TLockable<std::list<UdpServer>> ServerList; /*!< lockable list of UdpServer. */
    typedef TLockable<std::list<UdpClient>> ClientList; /*!< lockable list of UdpClient. */

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
     *  \return the server.
     */
    const UdpServer&  createServer(uint16_t port);

    /**
     *  \brief Binds the server on the given port and makes it listen for clients.
     *  \throw NetworkException can't bind the server port.
     *  \param server the server to launch.
     */
    void run(const UdpServer& server);

    /**
     *  \brief Close the socket bound to a specific port and all its clients.
     *  \param port bound port of the socket to close.
     */
    void  close(uint16_t port);

    /**
     *  \brief Close the socket and all its clients.
     *  \param server the server to close.
     */
    void  close(const UdpServer& server);

    /**
     *  \brief Blacklist an IP on a server.
     *  \param port port on which the server is bound.
     *  \param ip the IP to blacklist.
     */
    void  blacklist(uint16_t port, uint32_t ip);

  public:
    /**
     *  \brief create a UDP client to send datagrams to a remote UDP server.
     *  \param hostname hostname of the UDP server to connect to.
     *  \param port port of the UDP server to connect to.
     *  \return the client.
     */
    const UdpClient&  createClient(const std::string& hostname, uint16_t port);

    /**
     *  \brief Make the client start sending datagrams.
     *  \param client the client.
     */
    void run(const UdpClient& client);

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
    void  close(const UdpClient& client);

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
    void  recv(fd_set& set);

  private:
    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle<UdpSocketClient*>& event, UdpSocketClient *socket, const std::string& message);

    /**
     *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
     *  \param event the event to fire.
     *  \param socket the socket to send as argument of the event.
     *  \param message the message of the exception.
     */
    void  __onIOException(EventHandle<UdpSocketStream*>& event, UdpSocketStream *socket, const std::string& message);
  };
}

#endif    /* __CORE_NETWORK_UDP_UDPMANAGER_HH__ */
