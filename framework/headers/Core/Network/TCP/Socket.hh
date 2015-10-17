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
			protected:
				int	_fd;

			public:
				Socket(void);
				virtual ~Socket(void);

			public:
				virtual void	reinit(void);

			public:
				void		socket(void);
				void		close(void);
				void		connect(const std::string&, uint16_t);
				void		bind(uint16_t) const;
				void		listen(int) const;
				uint32_t	accept(Socket*) const;
				uint32_t	getpeername(void) const;
				uint32_t	getsockname(void) const;

			public:
				void	addToSet(fd_set&, int&) const;
				bool	isset(fd_set&) const;

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
					Pool(void);
					virtual ~Pool(void);

				public:
					void init(void);
				};
			};
		}
	}
}

#endif		/* __CORE_NETWORK_TCP_SOCKET_HH__ */