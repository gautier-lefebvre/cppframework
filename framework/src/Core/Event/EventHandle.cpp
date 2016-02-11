#include  "Core/Event/EventHandle.hh"
#include  "Core/Event/EventManager.hh"

using namespace fwk;

EventHandle::EventHandle(void):
  APooled<EventHandle>()
{}

void EventHandle::reinit(void) {}

void EventHandle::fireAsync(IEventArgs* args) const {
  EventManager::get().fireEventAsync(this, args);
}

void EventHandle::fireSync(IEventArgs* args) const {
  EventManager::get().fireEventSync(this, args);
}

void EventHandle::subscribe(const std::function<void (const IEventArgs*)>& callback, const void *key) const {
  EventManager::get().subscribeToEvent(this, callback, key);
}

void EventHandle::unsubscribe(const void *key) const {
  EventManager::get().unsubscribeFromEvent(this, key);
}

void EventHandle::registerToManager(void) const {
  EventManager::get().registerEvent(this);
}

void EventHandle::unregisterFromManager(void) const {
  EventManager::get().unregisterEvent(this);
}
