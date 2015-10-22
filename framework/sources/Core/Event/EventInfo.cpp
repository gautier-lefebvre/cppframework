#include  "Core/Event/EventInfo.hh"

Core::Event::EventInfo::EventInfo(const Core::Event::Event* ebase):
  base(ebase),
  cleanup(nullptr),
  subscribers()
{}

Core::Event::EventInfo::EventInfo(const Core::Event::Event* ebase, const std::function<void (Core::Event::IEventArgs*)>& clean):
  base(ebase),
  cleanup(clean),
  subscribers()
{}

Core::Event::EventInfo::EventInfo(const Core::Event::EventInfo& oth):
  base(oth.base),
  cleanup(oth.cleanup),
  subscribers(oth.subscribers)
{}

Core::Event::EventInfo& Core::Event::EventInfo::operator=(const Core::Event::EventInfo& oth) {
  if (this != &oth) {
    this->base = oth.base;
    this->cleanup = oth.cleanup;
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
