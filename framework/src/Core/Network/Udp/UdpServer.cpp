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
    EventHandle::getFromPool(),
    EventHandle::getFromPool(),
    EventHandle::getFromPool(),
    EventHandle::getFromPool()
  })
{}

UdpServer::~UdpServer(void) {
  EventHandle::returnToPool(this->events.onNewClient);
  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClientClosed);
  EventHandle::returnToPool(this->events.onClosed);
}
