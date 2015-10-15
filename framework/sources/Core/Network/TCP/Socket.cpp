#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netdb.h>
#include	<arpa/inet.h>
#include	<errno.h>
#include	<cstring>
#include	<unistd.h>

#include	"Core/Network/TCP/Socket.hh"
#include	"Core/Network/Exception.hh"
#include	"Library/Tool/Macro.hh"

Core::Network::TCP::Socket::Socket():
	Factory::AFactored(),
	Threading::Lock(),
	_fd(-1)
{}

Core::Network::TCP::Socket::~Socket() {
	this->reinit();
}

void	Core::Network::TCP::Socket::reinit() {
	SCOPELOCK(this);
	this->close();
}

void	Core::Network::TCP::Socket::socket() {
	SCOPELOCK(this);
	this->reinit();
	if ((this->_fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		throw Core::Network::Exception(std::string("socket: ") + strerror(errno));
	}
}

void	Core::Network::TCP::Socket::close() {
	SCOPELOCK(this);
	if (this->_fd != -1) {
		::close(this->_fd);
	}
	this->_fd = -1;
}

void	Core::Network::TCP::Socket::connect(const std::string &host, uint16_t port) {
	sockaddr_in	sin;
	hostent* h = gethostbyname(host.c_str());

	if (!h) {
		throw Core::Network::Exception(std::string("gethostbyname: ") + strerror(errno));
	}

	sin.sin_addr   = *(reinterpret_cast<in_addr*>(h->h_addr));
	sin.sin_port   = htons(port);
	sin.sin_family = AF_INET;

	if (::connect(this->_fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sockaddr)) == -1) {
		throw Core::Network::Exception(std::string("connect: ") + strerror(errno));
	}
}

void	Core::Network::TCP::Socket::bind(uint16_t port) const {
	if (port == 80) {
		throw Core::Network::Exception("bind: cannot bind port 80");
	}

	sockaddr_in sin;

	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_port        = htons(port);
	sin.sin_family      = AF_INET;

	if (::bind(this->_fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) == -1) {
		throw Core::Network::Exception(std::string("bind: ") + strerror(errno));
	}
}

void	Core::Network::TCP::Socket::listen(int nb) const {
	if (::listen(this->_fd, nb) == -1) {
		throw Core::Network::Exception(std::string("listen: ") + strerror(errno));
	}
}

uint32_t	Core::Network::TCP::Socket::accept(Core::Network::TCP::Socket* socket) const {
	sockaddr_in sin;
	socklen_t size = sizeof(sockaddr_in);

	if ((socket->_fd = ::accept(this->_fd, reinterpret_cast<sockaddr*>(&sin), &size)) == -1) {
		throw Core::Network::Exception(std::string("accept: ") + strerror(errno));
	}
	return static_cast<uint32_t>(sin.sin_addr.s_addr);
}

uint32_t	Core::Network::TCP::Socket::getpeername() const {
	sockaddr_in sin;
	socklen_t size = sizeof(sockaddr_in);
	if (::getpeername(this->_fd, reinterpret_cast<sockaddr*>(&sin), &size) == -1) {
		throw Core::Network::Exception(std::string("getpeername: ") + strerror(errno));
	}
	return static_cast<uint32_t>(sin.sin_addr.s_addr);
}

uint32_t	Core::Network::TCP::Socket::getsockname() const {
	sockaddr_in sin;
	socklen_t size = sizeof(sockaddr_in);
	if (::getsockname(this->_fd, reinterpret_cast<sockaddr*>(&sin), &size) == -1) {
		throw Core::Network::Exception(std::string("getsockname: ") + strerror(errno));
	}
	return static_cast<uint32_t>(sin.sin_addr.s_addr);
}

void	Core::Network::TCP::Socket::addToSet(fd_set& set, int& max) const {
	FD_SET(this->_fd, &set);
	max = MAX(max, this->_fd);
}

bool	Core::Network::TCP::Socket::isset(fd_set& set) const {
	return (FD_ISSET(this->_fd, &set) != 0);
}

/**
 *	Socket pool
 */

Core::Network::TCP::Socket::Pool::Pool():
	Factory::BasicPool<Core::Network::TCP::Socket>()
{}

Core::Network::TCP::Socket::Pool::~Pool() {}

void	Core::Network::TCP::Socket::Pool::init() {
	this->initPool(Core::Network::TCP::Socket::Pool::ORIGINAL_SIZE,
		Core::Network::TCP::Socket::Pool::HYDRATE_SIZE,
		"Core::Network::TCP::Socket");
}
