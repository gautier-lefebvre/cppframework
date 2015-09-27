#ifndef		__CORE_NETWORK_TCP_SOCKET_HH__
#define		__CORE_NETWORK_TCP_SOCKET_HH__

#include	<sys/select.h>

#include	"Library/Threading/Lock.hpp"
#include	"Library/Factory/IFactored.hh"

namespace			Core {
	namespace		Network {
		namespace	TCP {
			class	Socket :public Factory::IFactored, public Threading::Lock {
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