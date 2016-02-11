#include  <netdb.h>

#include  "Core/Network/Udp/UdpSocketStream.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

UdpSocketStream::UdpSocketStream(void):
  AUdpSocketIO(),
  AUdpSocket(),
  APooled<UdpSocketStream>()
{}

UdpSocketStream::~UdpSocketStream(void) {
  this->reinit();
}

void UdpSocketStream::reinit(void) {
  SCOPELOCK(this);
  this->AUdpSocketIO::reinit();
  this->AUdpSocket::reinit();
}

void UdpSocketStream::init(const std::string& hostname, uint16_t port) {
  SCOPELOCK(this);
  this->AUdpSocket::init();

  hostent *hostinfo = gethostbyname(hostname.c_str());
  if (hostinfo == nullptr) {
    throw NetworkException(std::string("gethostbyname: ") + strerror(errno));
  }

  this->_addr.sin_addr   = *(reinterpret_cast<in_addr*>(hostinfo->h_addr));
  this->_addr.sin_port   = htons(port);
  this->_addr.sin_family = AF_INET;
}

ssize_t UdpSocketStream::sendto(void) {
  SCOPELOCK(this);

  ByteArray* datagram = this->_output.first.front();
  this->_output.first.pop();
  this->_output.second -= datagram->getSize();

  ssize_t ret = ::sendto(this->_fd, datagram->getBytes(), datagram->getSize(), MSG_NOSIGNAL, reinterpret_cast<sockaddr*>(&(this->_addr)), sizeof(sockaddr_in));

  ByteArray::returnToPool(datagram);

  if (ret < 0) {
    throw NetworkException(std::string("sendto: ") + strerror(errno));
  }

  return ret;
}

ssize_t  UdpSocketStream::recvfrom(void) {
  SCOPELOCK(this);
  ByteArray* datagram = nullptr;
  socklen_t addrlen = sizeof(sockaddr_in);

  // read data in buffer
  ssize_t ret = ::recvfrom(this->_fd, this->_buffer->atStart(), this->_buffer->getSizeMax(), 0, reinterpret_cast<sockaddr*>(&(this->_addr)), &addrlen);

  if (ret < 0) {
    throw NetworkException(std::string("recvfrom: ") + strerror(errno));
  } else if (ret == 0) {
    throw NetworkException("udp socket closed");
  }

  if (this->_input.second + ret <= AUdpSocketIO::BUFFER_SIZE) {
    // copy buffer to datagram resized to the number of bytes read.
    datagram = ByteArray::getFromPool(ret, true);
    datagram->push(this->_buffer->atStart(), ret, false);

    // add datagram to datagram queue
    this->_input.first.push(datagram);
    this->_input.second += datagram->getSize();
    return ret;
  } else {
    throw NetworkException("recvfrom: input buffer is full");
  }
}
