#include  "Core/Network/Udp/UdpSocketServer.hh"
#include  "Core/Network/Udp/UdpSocketClient.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

UdpSocketServer::UdpSocketServer(void):
  AUdpSocket(),
  APooled<UdpSocketServer>()
{}

UdpSocketServer::~UdpSocketServer(void) {
  this->reinit();
}

void UdpSocketServer::reinit(void) {
  this->AUdpSocket::reinit();
}

void  UdpSocketServer::bind(uint16_t port) {
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

ssize_t UdpSocketServer::sendto(UdpSocketClient* client) {
  SCOPELOCK(this);

  ByteArray* datagram = client->nextDatagram();
  if (datagram == nullptr) {
    return 0;
  }

  ssize_t ret = ::sendto(this->_fd, datagram->getBytes(), datagram->getSize(), MSG_NOSIGNAL, reinterpret_cast<const sockaddr*>(&(client->socketAddress())), sizeof(sockaddr_in));

  ByteArray::returnToPool(datagram);

  if (ret < 0) {
    throw NetworkException(std::string("sendto: ") + strerror(errno));
  }

  return ret;
}

ByteArray*  UdpSocketServer::recvfrom(struct sockaddr_in& addr) {
  SCOPELOCK(this);
  ByteArray* datagram = nullptr;
  socklen_t addrlen = sizeof(sockaddr_in);

  // read data in buffer
  ssize_t ret = ::recvfrom(this->_fd, this->_buffer->atStart(), this->_buffer->getSizeMax(), 0, reinterpret_cast<sockaddr*>(&(addr)), &addrlen);

  if (ret < 0) {
    throw NetworkException(std::string("recvfrom: ") + strerror(errno));
  } else if (static_cast<size_t>(ret) > AUdpSocketIO::BUFFER_SIZE) {
    throw NetworkException(std::string("recvfrom: received a datagram bigger than the buffer size (discarded)"));
  }

  // copy buffer to datagram resized to the number of bytes read.
  size_t size = static_cast<size_t>(ret);

  datagram = ByteArray::getFromPool(size, true);
  datagram->push(this->_buffer->atStart(), size, false);

  return datagram;
}
