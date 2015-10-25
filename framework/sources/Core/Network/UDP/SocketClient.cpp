#include  <netdb.h>

#include  "Core/Network/UDP/SocketClient.hh"
#include  "Core/Network/Exception.hh"

Core::Network::UDP::SocketClient::SocketClient(void):
  Core::Network::UDP::ASocketIO(),
  _info(std::make_pair(0, 0))
{}

Core::Network::UDP::SocketClient::~SocketClient(void) {}

void  Core::Network::UDP::SocketClient::reinit(void) {
  SCOPELOCK(this);

  this->Core::Network::UDP::ASocketIO::reinit();

  // reinit socket information
  this->_info.first = 0;
  this->_info.second = 0;
}

void  Core::Network::UDP::SocketClient::init(const sockaddr_in& addr) {
  if (&(addr) != &(this->_addr)) {
    memcpy(&(this->_addr), &(addr), sizeof(sockaddr_in));
    this->_info.first = static_cast<uint32_t>(this->_addr.sin_addr.s_addr);
    this->_info.second = ntohs(this->_addr.sin_port);
  }
}

ByteArray*  Core::Network::UDP::ASocketIO::getData(void) {
  SCOPELOCK(this);

  if (!(this->_output.first.empty())) {
    ByteArray* datagram = this->_output.first.front();
    this->_output.first.pop();
    this->_output.second -= datagram->getSize();
    return datagram;
  } else {
    return nullptr;
  }
}

void  Core::Network::UDP::SocketClient::received(ByteArray* datagram) {
  if (datagram->getSize() + this->_input.second > Core::Network::UDP::ASocketIO::BUFFER_SIZE) {
    throw Core::Network::Exception("recvfrom: buffer is full");
  } else {
    this->_input.first.push(datagram);
    this->_input.second += datagram->getSize();
  }
}

const sockaddr_in& Core::Network::UDP::SocketClient::socketAddress(void) const {
  return this->_addr;
}

const std::pair<uint32_t, uint16_t>& Core::Network::UDP::SocketClient::clientInformation(void) const {
  return this->_info;
}
