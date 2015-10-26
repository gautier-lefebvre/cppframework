#include  "Core/Network/UDP/SocketServer.hh"
#include  "Core/Network/UDP/SocketClient.hh"

Core::Network::UDP::SocketServer::SocketServer(void):
  Core::Network::UDP::ASocket()
{}

Core::Network::UDP::SocketServer::~SocketServer(void) {
  this->reinit();
}

void Core::Network::UDP::SocketServer::init(void) {
  SCOPELOCK(this);
  this->reinit();
  this->_buffer = ByteArray::getFromPool(Core::Network::UDP::ASocketIO::BUFFER_SIZE);
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
  } else if (ret > Core::Network::UDP::ASocketIO::BUFFER_SIZE) {
    throw Core::Network::Exception(std::string("recvfrom: received a datagram bigger than the buffer size (discarded)"));
  }

  // copy buffer to datagram resized to the number of bytes read.
  datagram = ByteArray::getFromPool(ret, true);
  datagram->push(this->_buffer->atStart(), ret, false);

  return datagram;
}
