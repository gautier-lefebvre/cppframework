#include  "Core/Event/EventInfo.hh"

using namespace fwk;

Core::Event::EventInfo::EventInfo(const Core::Event::Handle* ebase):
  base(ebase),
  subscribers()
{}

Core::Event::EventInfo::EventInfo(const Core::Event::EventInfo& oth):
  base(oth.base),
  subscribers(oth.subscribers)
{}

Core::Event::EventInfo& Core::Event::EventInfo::operator=(const Core::Event::EventInfo& oth) {
  if (this != &oth) {
    this->base = oth.base;
    this->subscribers = oth.subscribers;
  }
  return *this;
}

void  Core::Event::EventInfo::addSubscriber(const void *subscriber, const std::function<void  (const Core::Event::IEventArgs*)>& callback) {
  this->subscribers[subscriber] = callback;
}

void  Core::Event::EventInfo::delSubscriber(const void *subscriber) {
  this->subscribers.erase(subscriber);
}
