#include  "Core/Network/Udp/UdpClient.hh"

using namespace fwk;

UdpClient::UdpClient(const std::string& hostname, uint16_t port, UdpSocketStream* socket):
  hostname(hostname),
  port(port),
  socket(socket),
  active(false),
  events({
    EventHandle<UdpSocketStream*>::getFromPool(),
    EventHandle<UdpSocketStream*>::getFromPool()
  })
{}

UdpClient::~UdpClient(void) {
  EventHandle<UdpSocketStream*>::returnToPool(this->events.onReceivedData);
  EventHandle<UdpSocketStream*>::returnToPool(this->events.onClosed);
}

