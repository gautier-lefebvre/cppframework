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
    EventHandle<TcpSocketStream*>::getFromPool(),
    EventHandle<TcpSocketStream*>::getFromPool(),
    EventHandle<TcpSocketStream*>::getFromPool(),
    EventHandle<TcpSocket*>::getFromPool()
  })
{}

TcpServer::~TcpServer(void) {
  EventHandle<TcpSocketStream*>::returnToPool(this->events.onAccept);
  EventHandle<TcpSocketStream*>::returnToPool(this->events.onReceivedData);
  EventHandle<TcpSocketStream*>::returnToPool(this->events.onClientClosed);
  EventHandle<TcpSocket*>::returnToPool(this->events.onClosed);
}
