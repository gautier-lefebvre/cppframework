#include  <sys/types.h>
#include  <sys/socket.h>
#include  <errno.h>
#include  <cstring>
#include  <string>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/Tcp/TcpSocketStream.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

const size_t TcpSocketStream::BUFFER_SIZE = 32768;

TcpSocketStream::TcpSocketStream(void):
  TcpSocket(),
  APooled<TcpSocketStream>(),
  _input(nullptr),
  _output(nullptr)
{}

TcpSocketStream::~TcpSocketStream(void) {}

void  TcpSocketStream::reinit(void) {
  SCOPELOCK(this);
  this->TcpSocket::reinit();
  ByteArray::returnToPool(this->_input);
  ByteArray::returnToPool(this->_output);
  this->_input = nullptr;
  this->_output = nullptr;
}

void  TcpSocketStream::init(void) {
  if (this->_input == nullptr) {
    this->_input = ByteArray::getFromPool(TcpSocketStream::BUFFER_SIZE);
  }
  if (this->_output == nullptr) {
    this->_output = ByteArray::getFromPool(TcpSocketStream::BUFFER_SIZE);
  }
}

bool  TcpSocketStream::hasDataToSend(void) const {
  return (!this->_output->empty());
}

void  TcpSocketStream::push(const void* buffer, size_t size) {
  try {
    SCOPELOCK(this);
    this->_output->push(buffer, size);
  } catch (const std::out_of_range& e) {
    throw NetworkException(e.what());
  }
}

ssize_t TcpSocketStream::recv(void) {
  SCOPELOCK(this);

  if (this->_input->full()) {
    throw NetworkException("recv: buffer is full");
  }

  ssize_t ret = ::recv(this->_fd, this->_input->atEnd(), this->_input->availableSpace(), 0);

  if (ret == -1) {
    throw NetworkException(std::string("recv: ") + strerror(errno));
  } else if (ret == 0) {
    throw NetworkException("connection closed");
  } else if (ret > 0) {
    this->_input->moveEnd(static_cast<size_t>(ret));
  }

  return ret;
}

ssize_t TcpSocketStream::send(void) {
  SCOPELOCK(this);
  ssize_t ret = ::send(this->_fd, this->_output->atStart(), this->_output->getSize(), MSG_NOSIGNAL);

  if (ret == -1) {
    throw NetworkException(std::string("send: ") + strerror(errno));
  } else if (ret > 0) {
    this->_output->clearStart(static_cast<size_t>(ret));
  }

  return ret;
}

size_t  TcpSocketStream::extractData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest) {
  SCOPELOCK(this);
  size_t size;

  if ((size = callback(*(this->_input))) != std::string::npos) {
    dest->resize(this->_input->getSize());
    dest->moveEnd(this->_input->extract(dest->atStart(), size));
  }

  return size;
}

size_t  TcpSocketStream::getData(const std::function<size_t (const ByteArray&)>& callback, ByteArray* dest) {
  SCOPELOCK(this);
  size_t size;

  if ((size = callback(*(this->_input))) != std::string::npos) {
    dest->resize(this->_input->getSize());
    dest->moveEnd(this->_input->get(dest->atStart(), size));
  }

  return size;
}

size_t  TcpSocketStream::seekData(const std::function<size_t (const ByteArray&)>& callback) {
  SCOPELOCK(this);
  return callback(*(this->_input));
}
