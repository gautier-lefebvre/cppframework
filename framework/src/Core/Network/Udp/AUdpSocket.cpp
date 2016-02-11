#include  <sys/types.h>
#include  <sys/socket.h>
#include  <unistd.h>

#include  "Library/Tool/Macro.hh"
#include  "Core/Network/Udp/AUdpSocket.hh"
#include  "Core/Network/Udp/AUdpSocketIO.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

AUdpSocket::AUdpSocket(void):
  Threading::Lockable(),
  Factory::AFactored(),
  _fd(-1),
  _buffer(nullptr)
{}

AUdpSocket::~AUdpSocket(void) {
  this->reinit();
}

void AUdpSocket::reinit(void) {
  SCOPELOCK(this);

  this->close();
  ByteArray::returnToPool(this->_buffer);
  this->_buffer = nullptr;
}

void AUdpSocket::init(void) {
  SCOPELOCK(this);
  this->_buffer = ByteArray::getFromPool(AUdpSocketIO::BUFFER_SIZE);
}

void AUdpSocket::socket(void) {
  SCOPELOCK(this);
  if ((this->_fd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw NetworkException(std::string("socket: ") + strerror(errno));
  }
}

void AUdpSocket::close(void) {
  SCOPELOCK(this);
  if (this->_fd != -1) {
    ::close(this->_fd);
  }
  this->_fd = -1;
}

void AUdpSocket::addToSet(fd_set& set, int& max) const {
  FD_SET(this->_fd, &set);
  max = MAX(max, this->_fd);
}

bool  AUdpSocket::isset(fd_set& set) const {
  return (FD_ISSET(this->_fd, &set) != 0);
}
