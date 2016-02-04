#include  "Core/Event/Handle.hh"
#include  "Core/Event/Manager.hh"

using namespace fwk;

void Core::Event::Handle::reinit(void) {}

void Core::Event::Handle::fireAsync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventAsync(this, args);
}

void Core::Event::Handle::fireSync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventSync(this, args);
}

void Core::Event::Handle::subscribe(const std::function<void (const Core::Event::IEventArgs *)>& callback, const void *key) const {
  Core::Event::Manager::get().subscribeToEvent(this, callback, key);
}

void Core::Event::Handle::unsubscribe(const void *key) const {
  Core::Event::Manager::get().unsubscribeFromEvent(this, key);
}

void Core::Event::Handle::registerToManager(void) const {
  Core::Event::Manager::get().registerEvent(this);
}

void Core::Event::Handle::unregisterFromManager(void) const {
  Core::Event::Manager::get().unregisterEvent(this);
}
