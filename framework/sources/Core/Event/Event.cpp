#include  "Core/Event/Event.hh"
#include  "Core/Event/Manager.hh"

void Core::Event::Event::reinit(void) {}

void Core::Event::Event::fireAsync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventAsync(this, args);
}

void Core::Event::Event::fireSync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventSync(this, args);
}
