#ifndef   __CORE_NETWORK_TCP_SOCKET_HH__
#define   __CORE_NETWORK_TCP_SOCKET_HH__

#include  <sys/select.h>

#include  "Library/Factory/AFactored.hh"
#include  "Library/Threading/Lock.hpp"

namespace     Core {
  namespace   Network {
    namespace TCP {
      /**
       *  \class Socket Core/Network/TCP/Socket.hh
       *  \brief A TCP socket.
       */
      class   Socket :public Factory::AFactored, public Threading::Lockable {
      protected:
        int _fd; /*!< fd of the socket */

      public:
        /**
         *  \brief Constructor of Socket.
         */
        Socket(void);

        /**
         *  \brief Destructor of Socket. Closes the socket.
         */
        virtual ~Socket(void);

      public:
        /**
         *  \brief Closes the socket.
         */
        virtual void  reinit(void);

      public:
        /**
         *  \brief Creates the socket.
         */
        void      socket(void);

        /**
         *  \brief Closes the socket.
         */
        void      close(void);

        /**
         *  \brief Connects to a given hostname:port.
         *  \throw Core::Network::Exception impossible to connect.
         *  \param hostname hostname to connect to.
         *  \param port port to connect to.
         */
        void      connect(const std::string& hostname, uint16_t port);

        /**
         *  \brief Binds a given port.
         *  \throw Core::Network::Exception impossible to bind the port.
         *  \param port port to bind.
         */
        void      bind(uint16_t port) const;

        /**
         *  \brief Listens on the bound port. Must be called after bind()
         *  \throw Core::Network::Exception impossible to listen.
         *  \param backlog see man listen.
         */
        void      listen(int backlog) const;

        /**
         *  \brief Accept a socket. Must be called after bind() et listen()
         *  \throw Core::Network::Exception impossible to accept.
         *  \param socket the socket which will be accepted.
         *  \return the IP of the accepted socket.
         */
        uint32_t  accept(Socket* socket) const;

        /**
         *  \brief see man getpeername.
         *  \throw Core::Network::Exception getpeername function call failed.
         */
        uint32_t  getpeername(void) const;

        /**
         *  \brief see man getsockname.
         *  \throw Core::Network::Exception getsockname function call failed.
         */
        uint32_t  getsockname(void) const;

      public:
        /**
         *  \brief Adds the socket fd to the set and update the max fd number.
         *  \param set the set.
         *  \param max_fd fd the max fd.
         */
        void      addToSet(fd_set& set, int& max_fd) const;

        /**
         *  \return true if the fd is in the set.
         */
        bool      isset(fd_set& set) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_TCP_SOCKET_HH__ */