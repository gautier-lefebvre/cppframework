#include  <netdb.h>

#include  "Core/Network/UDP/SocketStream.hh"
#include  "Core/Network/Exception.hh"

Core::Network::UDP::SocketStream::SocketStream(void):
  Core::Network::UDP::ASocketIO(),
  Core::Network::UDP::ASocket()
{}

Core::Network::UDP::SocketStream::~SocketStream(void) {
  this->reinit();
}

void Core::Network::UDP::SocketStream::reinit(void) {
  SCOPELOCK(this);
  this->Core::Network::UDP::ASocketIO::reinit();
  this->Core::Network::UDP::ASocket::reinit();
}

void Core::Network::UDP::SocketStream::init(const std::string& hostname, uint16_t port) {
  SCOPELOCK(this);

  hostent *hostinfo = gethostbyname(hostname.c_str());
  if (hostinfo == nullptr) {
    throw Core::Network::Exception(std::string("gethostbyname: ") + strerror(errno));
  }

  this->_addr.sin_addr   = *(reinterpret_cast<in_addr*>(hostinfo->h_addr));
  this->_addr.sin_port   = htons(port);
  this->_addr.sin_family = AF_INET;

  this->_buffer = ByteArray::getFromPool(Core::Network::UDP::ASocketIO::BUFFER_SIZE);
}

ssize_t Core::Network::UDP::SocketStream::sendto(void) {
  SCOPELOCK(this);

  ByteArray* datagram = this->_output.first.front();
  this->_output.first.pop();
  this->_output.second -= datagram->getSize();

  ssize_t ret = ::sendto(this->_fd, datagram->getBytes(), datagram->getSize(), MSG_NOSIGNAL, reinterpret_cast<sockaddr*>(&(this->_addr)), sizeof(sockaddr_in));

  ByteArray::returnToPool(datagram);

  if (ret < 0) {
    throw Core::Network::Exception(std::string("sendto: ") + strerror(errno));
  }

  return ret;
}

ssize_t  Core::Network::UDP::SocketStream::recvfrom(void) {
  SCOPELOCK(this);
  ByteArray* datagram = nullptr;
  socklen_t addrlen = sizeof(sockaddr_in);

  // read data in buffer
  ssize_t ret = ::recvfrom(this->_fd, this->_buffer->atStart(), this->_buffer->getSizeMax(), 0, reinterpret_cast<sockaddr*>(&(this->_addr)), &addrlen);

  if (ret < 0) {
    throw Core::Network::Exception(std::string("recvfrom: ") + strerror(errno));
  } else if (ret == 0) {
    throw Core::Network::Exception("udp socket closed");
  }

  // copy buffer to datagram resized to the number of bytes read.
  bool force = true;
  datagram = ByteArray::getFromPool(ret, force);
  datagram->push(this->_buffer->atStart(), ret, false);

  // add datagram to datagram queue
  this->_input.first.push(datagram);
  this->_input.second += datagram->getSize();

  return ret;
}
