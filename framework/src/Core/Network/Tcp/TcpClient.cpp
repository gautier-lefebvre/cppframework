#include  "Core/Event/EventManager.hh"
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
{
  EventManager& eventManager = EventManager::get();
  eventManager.registerEvent(this->events.onReceivedData);
  eventManager.registerEvent(this->events.onClosed);
}

TcpClient::~TcpClient(void) {
  EventManager& eventManager = EventManager::get();
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClosed);

  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClosed);
}
