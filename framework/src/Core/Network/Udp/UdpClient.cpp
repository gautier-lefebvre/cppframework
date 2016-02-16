#include  "Core/Network/Udp/UdpClient.hh"

using namespace fwk;

UdpClient::UdpClient(const std::string& hostname, uint16_t port, UdpSocketStream* socket):
  hostname(hostname),
  port(port),
  socket(socket),
  active(false),
  events({
    EventHandle::getFromPool(),
    EventHandle::getFromPool()
  })
{}

UdpClient::~UdpClient(void) {
  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClosed);
}

