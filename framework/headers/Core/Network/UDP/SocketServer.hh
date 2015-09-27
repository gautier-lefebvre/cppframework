#ifndef		__CORE_NETWORK_UDP_SOCKETSERVER_HH__
#define		__CORE_NETWORK_UDP_SOCKETSERVER_HH__

#include	<sys/select.h>

#include	"Library/Threading/Lock.hpp"
#include	"Library/Factory/IFactored.hh"

namespace			Core {
	namespace		Network {
		namespace	UDP {
			class	SocketServer :public Factory::IFactored, public Threading::Lock {
			private:
				int	_fd;

			public:
				SocketServer();
				virtual ~SocketServer();

			public:
				virtual void reinit();

			public:
				void	socket();
				void	close();
				void	bind();

			public:
				void	addToSet(fd_set&, int&) const;
				bool	isset(fd_set&) const;
			};
		}
	}
}

#endif		/* __CORE_NETWORK_UDP_SOCKETSERVER_HH__ */