#ifndef   __CORE_NETWORK_UDP_MANAGER_HH__
#define   __CORE_NETWORK_UDP_MANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Event/Event.hh"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Network/UDP/SocketServer.hh"
#include  "Core/Network/UDP/SocketClient.hh"
#include  "Core/Network/UDP/SocketStream.hh"

namespace Threading {
  struct  NotifiableThread;
}

namespace       Core {
  namespace     Network {
    /**
     *  \namespace UDP
     *  \brief UDP network classes.
     */
    namespace   UDP {
      /**
       *  \namespace EventArgs
       *  \brief Event arguments used in the UDP network
       */
      namespace EventArgs {
        /**
         *  \class SocketStreamArgs Core/Network/UDP/Manager.hh
         *  \brief Used when firing an event sending a Core::Network::UDP::SocketStream as argument.
         */
        struct  SocketStreamArgs :public Core::Event::IEventArgs, public Factory::TPooled<EventArgs::SocketStreamArgs, 20, 10> {
        public:
          Core::Network::UDP::SocketStream* socket; /*!< the SocketStream object. */

        public:
          /**
           *  \brief Constructor of SocketStreamArgs.
           */
          SocketStreamArgs(void);

        public:
          /**
           *  \brief Reinits the SocketStreamArgs object.
           */
          virtual void reinit(void);

          /**
           *  \brief Sends the SocketStream to its pool. Done automatically after the event has been fired.
           */
          virtual void cleanup(void);

          /**
           *  \brief Sets the SocketStream.
           *  \param ss the SocketStream.
           */
          virtual void init(Core::Network::UDP::SocketStream*);
        };

        /**
         *  \class SocketClientArgs Core/Network/UDP/Manager.hh
         *  \brief Used when firing an event sending a Core::Network::UDP::SocketClient as argument.
         */
        struct  SocketClientArgs :public Core::Event::IEventArgs, public Factory::TPooled<EventArgs::SocketClientArgs, 2, 1> {
        public:
          Core::Network::UDP::SocketClient* socket; /*!< the SocketClient object. */

        public:
          /**
           *  \brief Constructor of SocketClientArgs.
           */
          SocketClientArgs(void);

        public:
          /**
           *  \brief Reinits the SocketClientArgs object.
           */
          virtual void reinit(void);

          /**
           *  \brief Sends the SocketClient to its pool. Done automatically after the event has been fired.
           */
          virtual void cleanup(void);

          /**
           *  \brief Sets the SocketClient.
           *  \param ss the SocketClient.
           */
          virtual void init(Core::Network::UDP::SocketClient*);
        };

        /**
         *  \class SocketServerArgs Core/Network/UDP/Manager.hh
         *  \brief Used when firing an event sending a Core::Network::UDP::SocketServer as argument.
         */
        struct  SocketServerArgs :public Core::Event::IEventArgs, public Factory::TPooled<EventArgs::SocketServerArgs, 2, 1> {
        public:
          Core::Network::UDP::SocketServer* socket; /*!< the SocketServer object. */

        public:
          /**
           *  \brief Constructor of SocketServerArgs.
           */
          SocketServerArgs(void);

        public:
          /**
           *  \brief Reinits the SocketServerArgs object.
           */
          virtual void reinit(void);

          /**
           *  \brief Sends the SocketServer to its pool. Done automatically after the event has been fired.
           */
          virtual void cleanup(void);

          /**
           *  \brief Sets the SocketServer.
           *  \param ss the SocketServer.
           */
          virtual void init(Core::Network::UDP::SocketServer*);
        };
      }

      /**
       *  \class Manager Core/Network/UDP/Manager.hh
       *  \brief UDP Manager.
       */
      class   Manager {
      public:
        /**
         *  \class Server Core/Network/UDP/Manager.hh
         *  \brief UDP server and known clients.
         */
        struct  Server :public Threading::Lockable {
        public:
          uint16_t port; /*!< bound port */
          Core::Network::UDP::SocketServer* server; /*!< socket listening on the bound port */
          Threading::TLockable<std::list<Core::Network::UDP::SocketClient*>> clients; /*!< list of known clients to this server */
          std::set<uint32_t> accept; /*!< accepted IPs */
          std::set<uint32_t> blacklist; /*!< rejected IPs */

          struct Events {
            Core::Event::Event* onNewClient; /*!< Event fired whenever a new client sends a message to this server. Event argument type: Core::Network::UDP::EventArgs::SocketClientArgs. */
            Core::Event::Event* onReceivedData; /*!< Event fired whenever data is read from a client of this server. Event argument type: Core::Network::UDP::EventArgs::SocketClientArgs. */
            Core::Event::Event* onClientClosed; /*!< Event fired whenever a known client is removed. Does not work well as UDP is not a connected protocol. Event argument type: Core::Network::UDP::EventArgs::SocketClientArgs. */
            Core::Event::Event* onClosed; /*!< Event fired when this server is closed. Event argument type: Core::Network::UDP::EventArgs::SocketServerArgs. */
          } events; /*!< events for this server */

        public:
          /**
           *  \brief Constructor of Server.
           *  \param port bound port.
           *  \param socket socket listening to the bound port.
           *  \param accept list of IPs accepted by this server.
           *  \param blacklist list of IPs rejected by this server.
           */
          Server(uint16_t port, Core::Network::UDP::SocketServer* socket, const std::set<uint32_t>& accept = {}, const std::set<uint32_t>& blacklist = {});

          /**
           *  \brief Destructor of Server.
           */
          virtual ~Server(void);
        };

      public:
        /**
         *  \class Client Core/Network/UDP/Manager.hh
         *  \brief UDP client.
         */
        struct  Client :public Threading::Lockable {
        public:
          std::string hostname; /*!< hostname of the UDP socket this client sends messages to */
          uint16_t port; /*!< port of the UDP socket this client sends messages to */
          Core::Network::UDP::SocketStream *socket; /*!< socket */

          struct Events {
            Core::Event::Event* onReceivedData; /*!< Event fired whenever data is read from this socket. Event argument type: Core::Network::UDP::EventArgs::SocketStreamArgs. */
            Core::Event::Event* onClosed; /*!< Event fired when this socket is closed. Does not work well with UDP protocol. Event argument type: Core::Network::UDP::EventArgs::SocketStreamArgs. */
          } events; /*!< events for this client */

        public:
          /**
           *  \brief Constructor of Client.
           *  \param hostname the hostname of the UDP socket to send messages to.
           *  \param port the port of the UDP socket to send messages to.
           *  \param socket the socket.
           */
          Client(const std::string& hostname, uint16_t port, Core::Network::UDP::SocketStream* socket);

          /**
           *  \brief Destructor of Client.
           */
          virtual ~Client(void);
        };

      public:
        typedef Threading::TLockable<std::list<Server>> ServerList;
        typedef Threading::TLockable<std::list<Client>> ClientList;

      private:
        ServerList _servers; /*!< bound servers. */
        ClientList _clients; /*!< client sockets. */

        Threading::NotifiableThread& _input; /*!< input thread */
        Threading::NotifiableThread& _output; /*!< output thread */

      public:
        /**
         *  \brief Constructor of Manager.
         *  \param input the input thread.
         *  \param output the output thread.
         */
        Manager(Threading::NotifiableThread& input, Threading::NotifiableThread& output);

        /**
         *  \brief Destructor of Manager.
         */
        virtual ~Manager(void);

      public:
        /**
         *  \brief Close every server and client.
         */
        void clear(void);

      public:
        /**
         *  \brief Bind a socket to a specific port.
         *  \throw Core::Network::Exception can't bind this port.
         *  \param port the port to bind.
         *  \param accept the IPs to accept. Leave empty to accept any.
         *  \param blacklist the IPs to blacklist.
         *  \return the server.
         */
        const Server& bind(uint16_t port, const std::set<uint32_t>& accept = {}, const std::set<uint32_t>& blacklist = {});

        /**
         *  \brief Close the socket bound to a specific port and all its clients.
         *  \param port bound port of the socket to close.
         */
        void close(uint16_t port);

        /**
         *  \brief Close the socket and all its clients.
         *  \param server the server to close.
         */
        void close(const Server& server);

        /**
         *  \brief Blacklist an IP on a server.
         *  \param port port on which the server is bound.
         *  \param ip the IP to blacklist.
         */
        void blacklist(uint16_t port, uint32_t ip);

      public:
        /**
         *  \brief connect to a remote UDP server.
         *  \param hostname hostname of the UDP server to connect to.
         *  \param port port of the UDP server to connect to.
         *  \return the client.
         */
        const Client& connect(const std::string& hostname, uint16_t port);

        /**
         *  \brief Close the socket connected to a specific hostname:port.
         *  \param hostname the hostname the client is connected to.
         *  \param port the port the client is connected to.
         */
        void close(const std::string& hostname, uint16_t port);

        /**
         *  \brief Close the socket.
         *  \param client the client to close.
         */
        void close(const Client& client);

      public:
        /**
         *  \brief Push a new datagram to a socket. Closes the socket if its output buffer is full.
         *  \param socket the socket.
         *  \param ptr pointer to the start of the data to send.
         *  \param size the number of bytes of data to send.
         */
        void push(Core::Network::UDP::ASocketIO* socket, const void* ptr, size_t size);

        /**
         *  \brief Push a new datagram to a socket.
         *  \param socket the socket.
         *  \param bytearray the data to send.
         */
        void push(Core::Network::UDP::ASocketIO* socket, const ByteArray* bytearray);

      public:
        /**
         *  \brief Add every UDP socket to the set, increments the number of sockets add, and updates the max fd.
         *  \param set the set.
         *  \param max_fd the max fd number.
         *  \param nb the number of sockets added to the set.
         */
        void fillSetRead(fd_set& set, int& max_fd, uint32_t& nb);

        /**
         *  \brief Add every UDP socket to the set if it has data to send, increments the number of sockets add, and updates the max fd.
         *  \param set the set.
         *  \param max_fd the max fd number.
         *  \param nb the number of sockets added to the set.
         */
        void fillSetWrite(fd_set& set, int& max_fd, uint32_t& nb);

        /**
         *  \brief Send data of every socket in the set.
         *  \param set the set.
         */
        void send(fd_set& set);

        /**
         *  \brief Read from every socket in the set.
         *  \param set the set.
         */
        void recv(fd_set&);

      private:
        /**
         *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
         *  \param event the event to fire.
         *  \param socket the socket to send as argument of the event.
         *  \param message the message of the exception.
         */
        void __onIOException(Core::Event::Event *event, Core::Network::UDP::SocketClient *socket, const std::string& message);

        /**
         *  \brief Called when there is an I/O exception. Fires the given event with the socket as argument.
         *  \param event the event to fire.
         *  \param socket the socket to send as argument of the event.
         *  \param message the message of the exception.
         */
        void __onIOException(Core::Event::Event *event, Core::Network::UDP::SocketStream *socket, const std::string& message);

        /**
         *  \brief Fires an event with the socket as argument.
         *  \param event the event to fire.
         *  \param socket the socket to send as argument of the event.
         */
        void __fireEvent(Core::Event::Event *event, Core::Network::UDP::SocketStream *socket) const;

        /**
         *  \brief Fires an event with the socket as argument.
         *  \param event the event to fire.
         *  \param socket the socket to send as argument of the event.
         */
        void __fireEvent(Core::Event::Event *event, Core::Network::UDP::SocketClient *socket) const;

        /**
         *  \brief Fires an event with the socket as argument.
         *  \param event the event to fire.
         *  \param socket the socket to send as argument of the event.
         */
        void __fireEvent(Core::Event::Event *event, Core::Network::UDP::SocketServer *socket) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_MANAGER_HH__ */
