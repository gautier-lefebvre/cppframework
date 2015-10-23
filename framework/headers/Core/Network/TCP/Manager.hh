#ifndef   __CORE_NETWORK_TCP_MANAGER_HH__
#define   __CORE_NETWORK_TCP_MANAGER_HH__

#include  <list>
#include  <set>

#include  "Library/Threading/Lock.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Network/TCP/Socket.hh"
#include  "Core/Network/TCP/SocketStream.hh"
#include  "Core/Event/Event.hh"
#include  "Core/Event/IEventArgs.hh"

namespace       Core {
  namespace     Network {
    namespace   TCP {
      namespace EventArgs {
        struct  SocketStreamArgs :public Core::Event::IEventArgs, public Factory::HasBasicPool<onAccept::SocketStreamArgs, 20, 10> {
        public:
          Core::Network::TCP::SocketStream* socket;

        public:
          SocketStreamArgs(void);

        public:
          virtual void reinit(void);
          virtual void cleanup(void);
          void init(Core::Network::TCP::SocketStream*);
        };
      }

      class     Manager {
      private:
        struct  ServerClients :public Threading::Lock {
        public:
          uint16_t port;
          Core::Network::TCP::Socket* server;
          Threading::Lockable<Core::Network::TCP::SocketStream*>> clients;
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
        };
      };
    }
  }
}

#endif    /* __CORE_NETWORK_TCP_MANAGER_HH__ */
