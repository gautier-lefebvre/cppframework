#include  "Core/Event/EventManager.hh"
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
{
  EventManager& eventManager = EventManager::get();
  eventManager.registerEvent(this->events.onNewClient);
  eventManager.registerEvent(this->events.onReceivedData);
  eventManager.registerEvent(this->events.onClientClosed);
  eventManager.registerEvent(this->events.onClosed);
}

UdpServer::~UdpServer(void) {
  EventManager& eventManager = EventManager::get();
  eventManager.unregisterEvent(this->events.onNewClient);
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClientClosed);
  eventManager.unregisterEvent(this->events.onClosed);

  EventHandle::returnToPool(this->events.onNewClient);
  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClientClosed);
  EventHandle::returnToPool(this->events.onClosed);
}
