#include  <netdb.h>

#include  "Core/Network/Udp/UdpSocketClient.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

UdpSocketClient::UdpSocketClient(void):
  AUdpSocketIO(),
  APooled<UdpSocketClient>(),
  _info(std::make_pair(0, 0))
{}

UdpSocketClient::~UdpSocketClient(void) {}

void  UdpSocketClient::reinit(void) {
  SCOPELOCK(this);

  this->AUdpSocketIO::reinit();

  // reinit socket information
  this->_info.first = 0;
  this->_info.second = 0;
}

void  UdpSocketClient::init(const sockaddr_in& addr) {
  if (&(addr) != &(this->_addr)) {
    memcpy(&(this->_addr), &(addr), sizeof(sockaddr_in));
    this->_info.first = static_cast<uint32_t>(this->_addr.sin_addr.s_addr);
    this->_info.second = ntohs(this->_addr.sin_port);
  }
}

ByteArray*  UdpSocketClient::nextDatagram(void) {
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

void  UdpSocketClient::received(ByteArray* datagram) {
  if (datagram->getSize() + this->_input.second > AUdpSocketIO::BUFFER_SIZE) {
    throw NetworkException("recvfrom: buffer is full");
  } else {
    this->_input.first.push(datagram);
    this->_input.second += datagram->getSize();
  }
}

const sockaddr_in& UdpSocketClient::socketAddress(void) const {
  return this->_addr;
}

const std::pair<uint32_t, uint16_t>& UdpSocketClient::clientInformation(void) const {
  return this->_info;
}
