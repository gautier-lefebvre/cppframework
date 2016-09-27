#ifndef   __CORE_NETWORK_TCP_TCPSOCKET_HH__
#define   __CORE_NETWORK_TCP_TCPSOCKET_HH__

#include  <sys/select.h>

#include  "Library/Threading/Lockable.hpp"

namespace fwk {
  /**
   *  \class TcpSocket Core/Network/Tcp/TcpSocket.hh
   *  \brief A TCP socket.
   */
  class TcpSocket :public Lockable {
  protected:
    int _fd; /*!< fd of the socket */

  public:
    /**
     *  \brief Constructor of TcpSocket.
     */
    TcpSocket(void);

    /**
     *  \brief Destructor of TcpSocket. Closes the socket.
     */
    virtual ~TcpSocket(void);

  public:
    /**
     *  \brief Closes the socket.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Creates the socket.
     */
    void  socket(void);

    /**
     *  \brief Closes the socket.
     */
    void  close(void);

    /**
     *  \brief Connects to a given hostname:port.
     *  \throw NetworkException impossible to connect.
     *  \param hostname hostname to connect to.
     *  \param port port to connect to.
     */
    void  connect(const std::string& hostname, uint16_t port);

    /**
     *  \brief Binds a given port.
     *  \throw NetworkException impossible to bind the port.
     *  \param port port to bind.
     */
    void  bind(uint16_t port) const;

    /**
     *  \brief Listens on the bound port. Must be called after bind()
     *  \throw NetworkException impossible to listen.
     *  \param backlog see man listen.
     */
    void  listen(int backlog) const;

    /**
     *  \brief Accept a socket. Must be called after bind() et listen()
     *  \throw NetworkException impossible to accept.
     *  \param socket the socket which will be accepted.
     *  \return the IP of the accepted socket.
     */
    uint32_t  accept(TcpSocket* socket) const;

    /**
     *  \brief see man getpeername.
     *  \throw NetworkException getpeername function call failed.
     */
    uint32_t  getpeername(void) const;

    /**
     *  \brief see man getsockname.
     *  \throw NetworkException getsockname function call failed.
     */
    uint32_t  getsockname(void) const;

  public:
    /**
     *  \brief Adds the socket fd to the set and update the max fd number.
     *  \param set the set.
     *  \param max_fd fd the max fd.
     */
    void  addToSet(fd_set& set, int& max_fd) const;

    /**
     *  \return true if the fd is in the set.
     */
    bool  isset(fd_set& set) const;
  };
}

#endif    /* __CORE_NETWORK_TCP_TCPSOCKET_HH__ */
