#include  "Core/Event/Event.hh"
#include  "Core/Event/Manager.hh"

void Core::Event::Event::reinit(void) {}

void Core::Event::Event::fireAsync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventAsync(this, args);
}

void Core::Event::Event::fireSync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventSync(this, args);
}

void Core::Event::Event::subscribe(const std::function<void (const Core::Event::IEventArgs *)>& callback, const void *key) const {
  Core::Event::Manager::get().subscribeToEvent(this, callback, key);
}

void Core::Event::Event::unsubscribe(const void *key) const {
  Core::Event::Manager::get().unsubscribeFromEvent(this, key);
}

void Core::Event::Event::registerToManager(void) const {
  Core::Event::Manager::get().registerEvent(this);
}

void Core::Event::Event::unregisterFromManager(void) const {
  Core::Event::Manager::get().unregisterEvent(this);
}
