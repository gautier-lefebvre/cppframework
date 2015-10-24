#ifndef   __CORE_NETWORK_TCP_MANAGER_HH__
#define   __CORE_NETWORK_TCP_MANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/TCP/Socket.hh"
#include  "Core/Network/TCP/SocketStream.hh"
#include  "Core/Event/Event.hh"
#include  "Core/Event/IEventArgs.hh"

namespace Threading {
struct    NotifiableThread;
}

namespace       Core {
  namespace     Network {

    namespace   TCP {
      namespace EventArgs {
        struct  SocketStreamArgs :public Core::Event::IEventArgs, public Factory::HasBasicPool<EventArgs::SocketStreamArgs, 20, 10> {
        public:
          Core::Network::TCP::SocketStream* socket;

        public:
          SocketStreamArgs(void);

        public:
          virtual void reinit(void);
          virtual void cleanup(void);
          void init(Core::Network::TCP::SocketStream*);
        };

        struct  SocketArgs :public Core::Event::IEventArgs, public Factory::HasBasicPool<EventArgs::SocketArgs, 2, 1> {
        public:
          Core::Network::TCP::Socket* socket;

        public:
          SocketArgs(void);

        public:
          virtual void reinit(void);
          virtual void cleanup(void);
          void init(Core::Network::TCP::Socket*);
        };
      }

      class     Manager {
      public:
        struct  ServerClients :public Threading::Lock {
        public:
          uint16_t port;
          Core::Network::TCP::Socket* server;
          Threading::Lockable<std::list<Core::Network::TCP::SocketStream*>> clients;
          std::set<uint32_t> accept;
          std::set<uint32_t> blacklist;

          struct Events {
            Core::Event::Event* onAccept;
            Core::Event::Event* onReceivedData;
            Core::Event::Event* onClientClosed;
            Core::Event::Event* onClosed;
          } events;

        public:
          ServerClients(uint16_t, Core::Network::TCP::Socket*, const std::set<uint32_t>& = {}, const std::set<uint32_t>& = {});
          virtual ~ServerClients();
        };

      public:
        struct  RemoteConnection :public Threading::Lock {
        public:
          std::string hostname;
          uint16_t port;
          Core::Network::TCP::SocketStream *socket;

          struct Events {
            Core::Event::Event* onReceivedData;
            Core::Event::Event* onClosed;
          } events;

        public:
          RemoteConnection(const std::string&, uint16_t, Core::Network::TCP::SocketStream*);
          virtual ~RemoteConnection();
        };

      public:
        typedef Threading::Lockable<std::list<ServerClients>>    ServerList;
        typedef Threading::Lockable<std::list<RemoteConnection>> ConnectionList;

      private:
        ServerList     _servers;
        ConnectionList _connections;

        Threading::NotifiableThread& _input;
        Threading::NotifiableThread& _output;

      public:
        Manager(Threading::NotifiableThread&, Threading::NotifiableThread&);
        virtual ~Manager(void);

      public:
        void clear(void);

      public:
        const ServerClients& bind(uint16_t, const std::set<uint32_t>& = {}, const std::set<uint32_t>& = {});
        void close(uint16_t);
        void close(const ServerClients&);
        void blacklist(uint16_t, uint32_t);

      public:
        const RemoteConnection& connect(const std::string&, uint16_t);
        void close(const std::string&, uint16_t);
        void close(const RemoteConnection&);

      public:
        void push(Core::Network::TCP::SocketStream*, const void*, size_t);
        void push(Core::Network::TCP::SocketStream*, const ByteArray*);

      public:
        void fillSetRead(fd_set&, int&, uint32_t&);
        void fillSetWrite(fd_set&, int&, uint32_t&);
        void send(fd_set&);
        void recv(fd_set&);

      private:
        void __onIOException(Core::Event::Event*, Core::Network::TCP::SocketStream*, const std::string&);
        void __fireEvent(Core::Event::Event*, Core::Network::TCP::SocketStream*) const;
        void __fireEvent(Core::Event::Event*, Core::Network::TCP::Socket*) const;
      };
    }
  }
}

#endif    /* __CORE_NETWORK_TCP_MANAGER_HH__ */
