#include  "Core/Network/Tcp/TcpClient.hh"

using namespace fwk;

TcpClient::TcpClient(const std::string& hostname, uint16_t port, TcpSocketStream* socket):
  Lockable(),
  hostname(hostname),
  port(port),
  socket(socket),
  active(false),
  events({
    EventHandle<TcpSocketStream*>::getFromPool(),
    EventHandle<TcpSocketStream*>::getFromPool()
  })
{}

TcpClient::~TcpClient(void) {
  EventHandle<TcpSocketStream*>::returnToPool(this->events.onReceivedData);
  EventHandle<TcpSocketStream*>::returnToPool(this->events.onClosed);
}
