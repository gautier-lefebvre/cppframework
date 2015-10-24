#include  <sys/types.h>
#include  <sys/socket.h>
#include  <errno.h>
#include  <cstring>

#include  "Core/Network/TCP/SocketStream.hh"
#include  "Core/Network/Exception.hh"

const size_t Core::Network::TCP::SocketStream::BUFFER_SIZE = 32768;

Core::Network::TCP::SocketStream::SocketStream(void):
  Core::Network::TCP::Socket(),
  _input(nullptr),
  _output(nullptr)
{}

Core::Network::TCP::SocketStream::~SocketStream(void) {}

void  Core::Network::TCP::SocketStream::reinit(void) {
  SCOPELOCK(this);
  this->Core::Network::TCP::Socket::reinit();
  ByteArray::returnToPool(this->_input);
  ByteArray::returnToPool(this->_output);
  this->_input = nullptr;
  this->_output = nullptr;
}

void  Core::Network::TCP::SocketStream::init(void) {
  if (this->_input == nullptr) {
    this->_input = ByteArray::getFromPool(Core::Network::TCP::SocketStream::BUFFER_SIZE);
  }
  if (this->_output == nullptr) {
    this->_output = ByteArray::getFromPool(Core::Network::TCP::SocketStream::BUFFER_SIZE);
  }
}

bool  Core::Network::TCP::SocketStream::hasDataToSend(void) const {
  return (!this->_output->empty());
}

void  Core::Network::TCP::SocketStream::push(const void* buffer, size_t size) {
  try {
    SCOPELOCK(this);
    this->_output->push(buffer, size);
  } catch (const std::out_of_range& e) {
    throw Core::Network::Exception(e.what());
  }
}

ssize_t Core::Network::TCP::SocketStream::recv(void) {
  SCOPELOCK(this);

  if (this->_input->full()) {
    throw Core::Network::Exception("recv: buffer is full");
  }

  ssize_t ret = ::recv(this->_fd, this->_input->atEnd(), this->_input->availableSpace(), 0);

  if (ret == -1) {
    throw Core::Network::Exception(std::string("recv: ") + strerror(errno));
  } else if (ret == 0) {
    throw Core::Network::Exception("connection closed");
  } else if (ret > 0) {
    this->_input->moveEnd(static_cast<size_t>(ret));
  }

  return ret;
}

ssize_t Core::Network::TCP::SocketStream::send(void) {
  SCOPELOCK(this);
  ssize_t ret = ::send(this->_fd, this->_output->atStart(), this->_output->getSize(), MSG_NOSIGNAL);

  if (ret == -1) {
    throw Core::Network::Exception(std::string("send: ") + strerror(errno));
  } else if (ret > 0) {
    this->_output->clearStart(static_cast<size_t>(ret));
  }

  return ret;
}

size_t  Core::Network::TCP::SocketStream::getData(const std::function<size_t (ByteArray&, ByteArray&)>& dataGetter, ByteArray& dest) {
  SCOPELOCK(this);
  return dataGetter(*(this->_input), dest);
}
