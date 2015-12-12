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
    namespace   UDP {
      namespace EventArgs {
        struct  SocketStreamArgs :public Core::Event::IEventArgs, public Factory::TPooled<EventArgs::SocketStreamArgs, 20, 10> {
        public:
          Core::Network::UDP::SocketStream* socket;

        public:
          SocketStreamArgs(void);

        public:
          virtual void reinit(void);
          virtual void cleanup(void);
          void init(Core::Network::UDP::SocketStream*);
        };

        struct  SocketClientArgs :public Core::Event::IEventArgs, public Factory::TPooled<EventArgs::SocketClientArgs, 2, 1> {
        public:
          Core::Network::UDP::SocketClient* socket;

        public:
          SocketClientArgs(void);

        public:
          virtual void reinit(void);
          virtual void cleanup(void);
          void init(Core::Network::UDP::SocketClient*);
        };

        struct  SocketServerArgs :public Core::Event::IEventArgs, public Factory::TPooled<EventArgs::SocketServerArgs, 2, 1> {
        public:
          Core::Network::UDP::SocketServer* socket;

        public:
          SocketServerArgs(void);

        public:
          virtual void reinit(void);
          virtual void cleanup(void);
          void init(Core::Network::UDP::SocketServer*);
        };
      }

      class   Manager {
      public:
        struct  Server :public Threading::Lockable {
        public:
          uint16_t port;
          Core::Network::UDP::SocketServer* server;
          Threading::TLockable<std::list<Core::Network::UDP::SocketClient*>> clients;
          std::set<uint32_t> accept;
          std::set<uint32_t> blacklist;

          struct Events {
            Core::Event::Event* onNewClient;
            Core::Event::Event* onReceivedData;
            Core::Event::Event* onClientClosed;
            Core::Event::Event* onClosed;
          } events;

        public:
          Server(uint16_t, Core::Network::UDP::SocketServer*, const std::set<uint32_t>& = {}, const std::set<uint32_t>& = {});
          virtual ~Server(void);
        };

      public:
        struct  Client :public Threading::Lockable {
        public:
          std::string hostname;
          uint16_t port;
          Core::Network::UDP::SocketStream *socket;

          struct Events {
            Core::Event::Event* onReceivedData;
            Core::Event::Event* onClosed;
          } events;

        public:
          Client(const std::string&, uint16_t, Core::Network::UDP::SocketStream*);
          virtual ~Client(void);
        };

      public:
        typedef Threading::TLockable<std::list<Server>> ServerList;
        typedef Threading::TLockable<std::list<Client>> ClientList;

      private:
        ServerList _servers;
        ClientList _clients;

        Threading::NotifiableThread& _input;
        Threading::NotifiableThread& _output;

      public:
        Manager(Threading::NotifiableThread&, Threading::NotifiableThread&);
        virtual ~Manager(void);

      public:
        void clear(void);

      public:
        const Server& bind(uint16_t, const std::set<uint32_t>& = {}, const std::set<uint32_t>& = {});
        void close(uint16_t);
        void close(const Server&);
        void blacklist(uint16_t, uint32_t);

      public:
        const Client& connect(const std::string&, uint16_t);
        void close(const std::string&, uint16_t);
        void close(const Client&);

      public:
        void push(Core::Network::UDP::ASocketIO*, const void*, size_t);
        void push(Core::Network::UDP::ASocketIO*, const ByteArray*);

      public:
        void fillSetRead(fd_set&, int&, uint32_t&);
        void fillSetWrite(fd_set&, int&, uint32_t&);
        void send(fd_set&);
        void recv(fd_set&);

      private:
        void __onIOException(Core::Event::Event*, Core::Network::UDP::SocketClient*, const std::string&);
        void __onIOException(Core::Event::Event*, Core::Network::UDP::SocketStream*, const std::string&);
        void __fireEvent(Core::Event::Event*, Core::Network::UDP::SocketStream*) const;
        void __fireEvent(Core::Event::Event*, Core::Network::UDP::SocketClient*) const;
        void __fireEvent(Core::Event::Event*, Core::Network::UDP::SocketServer*) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_MANAGER_HH__ */
