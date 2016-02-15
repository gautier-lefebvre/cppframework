#include  "Core/Event/EventManager.hh"
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
{
  EventManager& eventManager = EventManager::get();
  eventManager.registerEvent(this->events.onAccept);
  eventManager.registerEvent(this->events.onReceivedData);
  eventManager.registerEvent(this->events.onClientClosed);
  eventManager.registerEvent(this->events.onClosed);
}

TcpServer::~TcpServer(void) {
  EventManager& eventManager = EventManager::get();
  eventManager.unregisterEvent(this->events.onAccept);
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClientClosed);
  eventManager.unregisterEvent(this->events.onClosed);

  EventHandle::returnToPool(this->events.onAccept);
  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClientClosed);
  EventHandle::returnToPool(this->events.onClosed);
}
