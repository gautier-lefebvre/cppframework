#ifndef		__CORE_NETWORK_HTTP_CLIENT_HH__
#define		__CORE_NETWORK_HTTP_CLIENT_HH__

#include	<vector>

#include	"Library/DesignPattern/Singleton.hpp"
#include	"Core/Network/HTTP/Connection.hh"

namespace			Core {
	namespace		Network {
		namespace	HTTP {
			class	Client:
				public Singleton<Core::Network::HTTP::Client>,
				public Threading::Lock,
				public AEndable {
				friend class Singleton<Core::Network::HTTP::Client>;
			private:
				std::string					_userAgent;
				std::vector<Connection*>	_connections;

			private:
				Client(const Client&) = delete;
				Client& operator=(const Client&) = delete;

			private:
				Client();
				virtual ~Client();

			public:
				virtual void	end();

			public:
				void	init(const std::string& user_agent);
				Connection*	getConnectionByHostPort(const std::string&, uint16_t = 80, uint16_t = 443, bool = true);
				void	sendRequest(Core::Network::HTTP::Request *, const std::string&, uint16_t = 80, uint16_t = 443);
				void	sendRequest(Core::Network::HTTP::Request *, Core::Network::HTTP::Connection *);
			};
		}
	}
}

#endif		/* __CORE_NETWORK_HTTP_CLIENT_HH__ */
