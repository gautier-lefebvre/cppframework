#include  "Core/Event/EventInfo.hh"

using namespace fwk;

EventInfo::EventInfo(const EventHandle* ebase):
  base(ebase),
  subscribers()
{}

EventInfo::EventInfo(const EventInfo& oth):
  base(oth.base),
  subscribers(oth.subscribers)
{}

EventInfo& EventInfo::operator=(const EventInfo& oth) {
  if (this != &oth) {
    this->base = oth.base;
    this->subscribers = oth.subscribers;
  }
  return *this;
}

void  EventInfo::addSubscriber(const void *subscriber, const std::function<void  (const IEventArgs*)>& callback) {
  this->subscribers[subscriber] = callback;
}

void  EventInfo::delSubscriber(const void *subscriber) {
  this->subscribers.erase(subscriber);
}
