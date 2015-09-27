#ifndef		__CORE_NETWORK_UDP_SOCKETCLIENT_HH__
#define		__CORE_NETWORK_UDP_SOCKETCLIENT_HH__

#include	<sys/select.h>
#include	<queue>

#include	"Library/Threading/Lock.hpp"
#include	"Library/Factory/IFactored.hh"

namespace			Core {
	namespace		Network {
		namespace	UDP {
			class	SocketClient :public Factory::AFactored, public Threading::Lock {
			private:
				sockaddr_in	_addr;
				std::pair<uint32_t, uint16_t> _info; // IP, port
				std::queue<ByteArray*>	_input;
				std::queue<ByteArray*>	_output;

			public:
				SocketClient();
				virtual ~SocketClient();

			public:
				virtual void reinit();

			public:
				void	init(const sockaddr_in&);

			public:
				void	addToSet(fd_set&, int&) const;
				bool	isset(fd_set&) const;

			public:
				const std::pair<uint32_t, uint16_t>& clientInformation() const;
			};
		}
	}
}

#endif		/* __CORE_NETWORK_UDP_SOCKETCLIENT_HH__ */
