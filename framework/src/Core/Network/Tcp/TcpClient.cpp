#include  "Core/Network/Tcp/TcpClient.hh"

using namespace fwk;

TcpClient::TcpClient(const std::string& hostname, uint16_t port, TcpSocketStream* socket):
  Lockable(),
  hostname(hostname),
  port(port),
  socket(socket),
  active(false),
  events({
    EventHandle::getFromPool(),
    EventHandle::getFromPool()
  })
{}

TcpClient::~TcpClient(void) {
  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClosed);
}
