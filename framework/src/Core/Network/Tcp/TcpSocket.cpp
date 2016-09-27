#include  <sys/types.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <arpa/inet.h>
#include  <errno.h>
#include  <cstring>
#include  <unistd.h>

#include  "Core/Network/Tcp/TcpSocket.hh"
#include  "Core/Network/Exception.hh"
#include  "Library/Tool/Macro.hh"

using namespace fwk;

TcpSocket::TcpSocket(void):
    Lockable(),
    _fd(-1)
{}

TcpSocket::~TcpSocket(void) {
    this->reinit();
}

void  TcpSocket::reinit(void) {
    SCOPELOCK(this);
    this->close();
}

void  TcpSocket::socket(void) {
    SCOPELOCK(this);
    if ((this->_fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        throw NetworkException(std::string("socket: ") + strerror(errno));
    }
}

void  TcpSocket::close(void) {
    SCOPELOCK(this);

    if (this->_fd != -1) {
        ::close(this->_fd);
    }

    this->_fd = -1;
}

void  TcpSocket::connect(const std::string &host, uint16_t port) {
    sockaddr_in  sin;
    hostent* h = gethostbyname(host.c_str());

    if (!h) {
        throw NetworkException(std::string("gethostbyname: ") + strerror(errno));
    }

    sin.sin_addr   = *(reinterpret_cast<in_addr*>(h->h_addr));
    sin.sin_port   = htons(port);
    sin.sin_family = AF_INET;

    if (::connect(this->_fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sockaddr)) == -1) {
        throw NetworkException(std::string("connect: ") + strerror(errno));
    }
}

void  TcpSocket::bind(uint16_t port) const {
    if (port == 80) {
        throw NetworkException("bind: cannot bind port 80");
    }

    sockaddr_in sin;

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port        = htons(port);
    sin.sin_family      = AF_INET;

    if (::bind(this->_fd, reinterpret_cast<sockaddr*>(&sin), sizeof(sin)) == -1) {
        throw NetworkException(std::string("bind: ") + strerror(errno));
    }
}

void  TcpSocket::listen(int nb) const {
    if (::listen(this->_fd, nb) == -1) {
        throw NetworkException(std::string("listen: ") + strerror(errno));
    }
}

uint32_t  TcpSocket::accept(TcpSocket* socket) const {
    sockaddr_in sin;
    socklen_t size = sizeof(sockaddr_in);

    if ((socket->_fd = ::accept(this->_fd, reinterpret_cast<sockaddr*>(&sin), &size)) == -1) {
        throw NetworkException(std::string("accept: ") + strerror(errno));
    }

    return static_cast<uint32_t>(sin.sin_addr.s_addr);
}

uint32_t  TcpSocket::getpeername(void) const {
    sockaddr_in sin;
    socklen_t size = sizeof(sockaddr_in);

    if (::getpeername(this->_fd, reinterpret_cast<sockaddr*>(&sin), &size) == -1) {
        throw NetworkException(std::string("getpeername: ") + strerror(errno));
    }

    return static_cast<uint32_t>(sin.sin_addr.s_addr);
}

uint32_t  TcpSocket::getsockname(void) const {
    sockaddr_in sin;
    socklen_t size = sizeof(sockaddr_in);

    if (::getsockname(this->_fd, reinterpret_cast<sockaddr*>(&sin), &size) == -1) {
        throw NetworkException(std::string("getsockname: ") + strerror(errno));
    }

    return static_cast<uint32_t>(sin.sin_addr.s_addr);
}

void  TcpSocket::addToSet(fd_set& set, int& max) const {
    FD_SET(this->_fd, &set);
    max = MAX(max, this->_fd);
}

bool  TcpSocket::isset(fd_set& set) const {
    return (FD_ISSET(this->_fd, &set) != 0);
}
