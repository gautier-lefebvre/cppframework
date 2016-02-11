#ifndef   __CORE_NETWORK_UDP_AUDPSOCKET_HH__
#define   __CORE_NETWORK_UDP_AUDPSOCKET_HH__

#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Factory/AFactored.hh"
#include  "Library/Threading/Lock.hpp"

namespace fwk {
  /**
   *  \class AUdpSocket Core/Network/Udp/AUdpSocket.hh
   *  \brief Superclass of UDP sockets which can be created.
   */
  class AUdpSocket :public virtual Threading::Lockable, public virtual Factory::AFactored {
  protected:
    int _fd; /*!< file descriptor of the socket */
    ByteArray* _buffer; /*!< buffer used to read */

  public:
    /**
     *  \brief Constructor of AUdpSocket.
     */
    AUdpSocket(void);

    /**
     *  \brief Destructor of AUdpSocket.
     */
    virtual ~AUdpSocket(void);

  public:
    /**
     *  \brief Closes the socket. Sends the buffer back to the pool.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Gets the buffer from the ByteArray pool.
     */
    virtual void  init(void);

  public:
    /**
     *  \brief Creates the DGRAM socket.
     */
    virtual void  socket(void);

    /**
     *  \brief Closes the socket.
     */
    virtual void  close(void);

  public:
    /**
     *  \brief Adds the socket fd to the set and update the max fd number.
     *  \param set the set.
     *  \param max_fd fd the max fd.
     */
    virtual void  addToSet(fd_set& set, int& max_fd) const;

    /**
     *  \return true if the fd is in the set.
     */
    virtual bool  isset(fd_set&) const;
  };
}

#endif    /* __CORE_NETWORK_UDP_AUDPSOCKET_HH__ */
