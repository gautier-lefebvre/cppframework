#include  "Core/Network/UDP/SocketServer.hh"
#include  "Core/Network/UDP/SocketClient.hh"

Core::Network::UDP::SocketServer::SocketServer(void):
  Factory::AFactored(),
  Threading::Lock(),
  _fd(-1)
{}

Core::Network::UDP::SocketServer::~SocketServer(void) {
  this->reinit();
}

void Core::Network::UDP::SocketServer::reinit(void) {
  SCOPELOCK(this);
  this->close();

  ByteArray::returnToPool(this->_buffer);
  this->_buffer = nullptr;
}

void Core::Network::UDP::SocketServer::init(void) {
  SCOPELOCK(this);
  this->reinit();
  this->_buffer = ByteArray::getFromPool(Core::Network::UDP::ASocketIO::BUFFER_SIZE);
}

void Core::Network::UDP::SocketServer::socket(void) {
  SCOPELOCK(this);
  this->reinit();
  if ((this->_fd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw Core::Network::Exception(std::string("socket: ") + strerror(errno));
  }
}

void Core::Network::UDP::SocketServer::close(void) {
  SCOPELOCK(this);
  if (this->_fd != -1) {
    ::close(this->_fd);
  }
  this->_fd = -1;
}

void  Core::Network::UDP::SocketServer::bind(uint16_t port) const {
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

ssize_t Core::Network::UDP::SocketServer::sendto(SocketClient* client) {
  SCOPELOCK(this);

  ByteArray* datagram = client->nextDatagram();
  if (datagram == nullptr) {
    return 0;
  }

  ssize_t ret = ::sendto(this->_fd, datagram->getBytes(), datagram->getSize(), MSG_NOSIGNAL, reinterpret_cast<sockaddr*>(&(client->socketAddress())), sizeof(sockaddr_in));

  ByteArray::returnToPool(datagram);

  if (ret < 0) {
    throw Core::Network::Exception(std::string("sendto: " + strerror(errno)));
  }

  return ret;
}

ByteArray*  Core::Network::UDP::SocketServer::recvfrom(struct sockaddr_in& addr) {
  SCOPELOCK(this);
  ByteArray* datagram = nullptr;
  socklen_t addrlen = sizeof(sockaddr_in);

  // read data in buffer
  ssize_t ret = ::recvfrom(this->_fd, this->_buffer->atStart(), this->_buffer->getSizeMax(), 0, reinterpret_cast<sockaddr*>(&(addr)), &addrlen);

  if (ret < 0) {
    throw Core::Network::Exception(std::string("recvfrom: ") + strerror(errno));
  }

  // copy buffer to datagram resized to the number of bytes read.
  datagram = ByteArray::getFromPool(ret, true);
  datagram->push(this->_buffer->atStart(), ret, false);

  return datagram;
}

void Core::Network::UDP::SocketServer::addToSet(fd_set& set, int& max) const {
  FD_SET(this->_fd, &set);
  max = MAX(max, this->_fd);
}

bool  Core::Network::UDP::SocketServer::isset(fd_set& set) const {
  return (FD_ISSET(this->_fd, &set) != 0);
}
