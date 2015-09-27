#ifndef		__CORE_NETWORK_TCP_SOCKETSTREAM_HH__
#define		__CORE_NETWORK_TCP_SOCKETSTREAM_HH__

#include	"Library/Collection/ByteArray.hpp"
#include	"Core/Network/TCP/Socket.hh"

namespace			Core {
	namespace		Network {
		namespace	TCP {
			class	SocketStream :public Socket {
			protected:
				ByteArray	*_input;
				ByteArray	*_output;

			public:
				static const size_t	BUFFER_SIZE = 32768;

			public:
				SocketStream();
				virtual ~SocketStream();

			public:
				virtual void	reinit();

			public:
				void	init();
				bool	hasDataToSend() const;
				void	push(const void*, size_t);
				void	recv();
				void	send();
			};
		}
	}
}

#endif		/* __CORE_NETWORK_TCP_SOCKETSTREAM_HH__ */