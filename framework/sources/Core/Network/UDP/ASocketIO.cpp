#include  "Core/Network/UDP/ASocketIO.hh"
#include  "Core/Network/Exception.hh"

const size_t Core::Network::UDP::ASocketIO::BUFFER_SIZE = 32768;

Core::Network::UDP::ASocketIO::ASocketIO(void):
  Threading::Lockable(),
  Factory::AFactored(),
  _addr(),
  _input(std::make_pair(std::queue<ByteArray*>(), 0)),
  _output(std::make_pair(std::queue<ByteArray*>(), 0))
{}

Core::Network::UDP::ASocketIO::~ASocketIO(void) {
  this->reinit();
}

void Core::Network::UDP::ASocketIO::reinit(void) {
  SCOPELOCK(this);

  // reinit socket address
  memset(&(this->_addr), 0, sizeof(sockaddr_in));

  while (!(this->_input.first.empty())) {
    ByteArray* bytearray = this->_input.first.front();
    this->_input.first.pop();
    ByteArray::returnToPool(bytearray);
  }

  while (!(this->_output.first.empty())) {
    ByteArray* bytearray = this->_output.first.front();
    this->_output.first.pop();
    ByteArray::returnToPool(bytearray);
  }

  this->_input.second  = 0;
  this->_output.second = 0;
}

bool  Core::Network::UDP::ASocketIO::hasDataToSend(void) const {
  return this->_output.second > 0;
}

void  Core::Network::UDP::ASocketIO::push(ByteArray* datagram) {
  SCOPELOCK(this);

  if (this->_output.second + datagram->getSize() > Core::Network::UDP::ASocketIO::BUFFER_SIZE) {
    throw Core::Network::Exception("ASocketIO::push: buffer maximum size reached");
  }

  this->_output.first.push(datagram);
  this->_output.second += datagram->getSize();
}

ByteArray*  Core::Network::UDP::ASocketIO::getData(void) {
  SCOPELOCK(this);

  if (!(this->_input.first.empty())) {
    ByteArray* datagram = this->_input.first.front();
    this->_input.first.pop();
    this->_input.second -= datagram->getSize();
    return datagram;
  } else {
    return nullptr;
  }
}

bool Core::Network::UDP::ASocketIO::operator==(const sockaddr_in& oth) const {
  return (oth.sin_family      == AF_INET && 
          oth.sin_family      == this->_addr.sin_family &&
          oth.sin_addr.s_addr == this->_addr.sin_addr.s_addr &&
          oth.sin_port        == this->_addr.sin_port);
}
