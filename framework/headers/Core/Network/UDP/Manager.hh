#ifndef   __CORE_NETWORK_UDP_MANAGER_HH__
#define   __CORE_NETWORK_UDP_MANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Event/Event.hh"
#include  "Core/Network/UDP/SocketServer.hh"
#include  "Core/Network/UDP/SocketClient.hh"
#include  "Core/Network/UDP/SocketStream.hh"

namespace Threading {
  struct  NotifiableThread;
}

namespace     Core {
  namespace   Network {
    namespace UDP {
      class   Manager {
      public:
        struct  Server :public Threading::Lock {
        public:
          uint16_t port;
          Core::Network::UDP::SocketServer* server;
          Threading::Lockable<std::list<Core::Network::UDP::SocketClient*>> clients;
          std::set<uint32_t> accept;
          std::set<uint32_t> blacklist;

          struct Events {
            Core::Event::Event* onAccept;
            Core::Event::Event* onReceivedData;
            Core::Event::Event* onClientClosed;
            Core::Event::Event* onClosed;
          } events;

        public:
          Server(uint16_t, Core::Network::UDP::SocketServer*, const std::set<uint32_t>& = {}, const std::set<uint32_t>& = {});
          virtual ~Server(void);
        };

      public:
        struct  Client :public Threading::Lock {
        public:
          std::string hostname;
          uint16_t port;
          Core::Network::UDP::SocketStream *socket;

          struct Events {
            Core::Event::Event* onReceivedData;
            Core::Event::Event* onClosed;
          } events;

        public:
          Client(const std::string&, uint16_t, Core::Network::TCP::SocketStream*);
          virtual ~Client(void);
        };

      public:
        typedef Threading::Lockable<std::list<Server>> ServerList;
        typedef Threading::Lockable<std::list<Client>> ClientList;

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
      };
    }
  }
}

#endif    /* __CORE_NETWORK_UDP_MANAGER_HH__ */
