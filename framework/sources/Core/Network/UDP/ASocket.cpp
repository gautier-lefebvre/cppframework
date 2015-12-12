#include  <sys/types.h>
#include  <sys/socket.h>
#include  <unistd.h>

#include  "Library/Tool/Macro.hh"
#include  "Core/Network/UDP/ASocket.hh"
#include  "Core/Network/Exception.hh"

Core::Network::UDP::ASocket::ASocket(void):
  Threading::Lockable(),
  Factory::AFactored(),
  _fd(-1),
  _buffer(nullptr)
{}

Core::Network::UDP::ASocket::~ASocket(void) {
  this->reinit();
}

void Core::Network::UDP::ASocket::reinit(void) {
  SCOPELOCK(this);

  this->close();
  ByteArray::returnToPool(this->_buffer);
  this->_buffer = nullptr;
}

void Core::Network::UDP::ASocket::socket(void) {
  SCOPELOCK(this);
  this->reinit();
  if ((this->_fd = ::socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    throw Core::Network::Exception(std::string("socket: ") + strerror(errno));
  }
}

void Core::Network::UDP::ASocket::close(void) {
  SCOPELOCK(this);
  if (this->_fd != -1) {
    ::close(this->_fd);
  }
  this->_fd = -1;
}

void Core::Network::UDP::ASocket::addToSet(fd_set& set, int& max) const {
  FD_SET(this->_fd, &set);
  max = MAX(max, this->_fd);
}

bool  Core::Network::UDP::ASocket::isset(fd_set& set) const {
  return (FD_ISSET(this->_fd, &set) != 0);
}
