#include  "Core/Event/EventManager.hh"
#include  "Core/Network/Udp/UdpClient.hh"

using namespace fwk;

UdpClient::UdpClient(const std::string& hostname, uint16_t port, UdpSocketStream* socket):
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

UdpClient::~UdpClient(void) {
  EventManager& eventManager = EventManager::get();
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClosed);

  EventHandle::returnToPool(this->events.onReceivedData);
  EventHandle::returnToPool(this->events.onClosed);
}

