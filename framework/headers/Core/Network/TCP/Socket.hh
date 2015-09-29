#ifndef		__CORE_NETWORK_TCP_SOCKET_HH__
#define		__CORE_NETWORK_TCP_SOCKET_HH__

#include	<sys/select.h>

#include	"Library/DesignPattern/Singleton.hpp"
#include	"Library/Factory/AFactored.hh"
#include	"Library/Factory/Pool.hpp"
#include	"Library/Threading/Lock.hpp"

namespace			Core {
	namespace		Network {
		namespace	TCP {
			class	Socket :public Factory::AFactored, public Threading::Lock {
			public:
				struct	Pool :public Singleton<Core::Network::TCP::Socket::Pool>, public Factory::BasicPool<Core::Network::TCP::Socket> {
					friend class Singleton<Core::Network::TCP::Socket::Pool>;
				public:
					const size_t	ORIGINAL_SIZE = 10;
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
				int	_fd;

			public:
				Socket();
				virtual ~Socket();

			public:
				virtual void	reinit();

			public:
				void		socket();
				void		close();
				void		connect(const std::string&, uint16_t);
				void		bind(uint16_t) const;
				void		listen(int) const;
				uint32_t	accept(Socket*) const;
				uint32_t	getpeername() const;
				uint32_t	getsockname() const;

			public:
				void	addToSet(fd_set&, int&) const;
				bool	isset(fd_set&) const;
			};
		}
	}
}

#endif		/* __CORE_NETWORK_TCP_SOCKET_HH__ */