#include  "Core/Network/Udp/UdpServer.hh"

using namespace fwk;

UdpServer::UdpServer(uint16_t port, UdpSocketServer* server):
  Lockable(),
  port(port),
  server(server),
  clients(),
  accept(),
  blacklist(),
  active(false),
  events({
    EventHandle<UdpSocketClient*>::getFromPool(),
    EventHandle<UdpSocketClient*>::getFromPool(),
    EventHandle<UdpSocketClient*>::getFromPool(),
    EventHandle<UdpSocketServer*>::getFromPool()
  })
{}

UdpServer::~UdpServer(void) {
  EventHandle<UdpSocketClient*>::returnToPool(this->events.onNewClient);
  EventHandle<UdpSocketClient*>::returnToPool(this->events.onReceivedData);
  EventHandle<UdpSocketClient*>::returnToPool(this->events.onClientClosed);
  EventHandle<UdpSocketServer*>::returnToPool(this->events.onClosed);
}
