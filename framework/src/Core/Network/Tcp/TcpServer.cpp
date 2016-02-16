#include  "Core/Network/Tcp/TcpServer.hh"

using namespace fwk;

TcpServer::TcpServer(uint16_t port, TcpSocket* server):
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

TcpServer::~TcpServer(void) {
  EventHandle::returnToPool(this->events.onAccept);
  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClientClosed);
  EventHandle::returnToPool(this->events.onClosed);
}
