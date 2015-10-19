#ifndef		__CORE_NETWORK_TCP_MANAGER_HH__
#define		__CORE_NETWORK_TCP_MANAGER_HH__

#include	<list>
#include	<set>

#include	"Library/Threading/Lock.hpp"
#include	"Core/Network/TCP/Socket.hh"
#include	"Core/Network/TCP/SocketStream.hh"
#include	"Core/Event/Event.hh"

namespace			Core {
	namespace		Network {
		namespace	TCP {
			class	Manager {
			private:
				struct ServerClients :public Threading::Lock {
				public:
					uint16_t port;
					Core::Network::TCP::Socket*	server;
					Threading::Lockable<Core::Network::TCP::SocketStream*>> clients;
					std::set<uint32_t>	accept;
					std::set<uint32_t>	blacklist;

					struct {
						Core::Event::Event	onAccept;
						Core::Event::Event	onReceivedData;
						Core::Event::Event	onClientClosed;
						Core::Event::Event	onClosed;
					} events;

				public:
					ServerClients(uint16_t, Core::Network::TCP::Socket*, const std::set<uint32_t>& = {}, const std::set<uint32_t>& = {});
					ServerClients(const ServerClients&);
				};

				struct RemoteConnection :public Threading::Lock {
				public:
					std::string hostname;
					uint16_t port;
					Core::Network::TCP::SocketStream *socket;

					struct {
						Core::Event::Event	onReceivedData;
						Core::Event::Event	onClosed;
					} events;

				public:
					RemoteConnection(const std::string&, uint16_t, Core::Network::TCP::SocketStream*);
					RemoteConnection(const RemoteConnection&);
				};
			};
		}
	}
}

#endif		/* __CORE_NETWORK_TCP_MANAGER_HH__ */
