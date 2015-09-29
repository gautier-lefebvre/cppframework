#ifndef		__CORE_NETWORK_TCP_SOCKETSTREAM_HH__
#define		__CORE_NETWORK_TCP_SOCKETSTREAM_HH__

#include	"Library/Collection/ByteArray.hpp"
#include	"Core/Network/TCP/Socket.hh"

namespace			Core {
	namespace		Network {
		namespace	TCP {
			class	SocketStream :public Socket {
			public:
				struct	Pool :public Singleton<Core::Network::TCP::SocketStream::Pool>, public Factory::BasicPool<Core::Network::TCP::SocketStream> {
					friend class Singleton<Core::Network::TCP::SocketStream::Pool>;
				public:
					const size_t	ORIGINAL_SIZE = 100;
					const size_t	HYDRATE_SIZE = 10;

				private:
					Pool(const Pool&) = delete;
					Pool(const Pool&&) = delete;
					Pool& operator=(const Pool&) = delete;

				private:
					Pool();
					virtual ~Pool();

				public:
					void init();
				};

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
