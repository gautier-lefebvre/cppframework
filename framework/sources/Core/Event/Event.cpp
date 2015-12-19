#include  "Core/Event/Event.hh"
#include  "Core/Event/Manager.hh"

Core::Event::Event::Event(void):
  _source(Core::Event::Source::UNSET)
{}

Core::Event::Event::~Event(void) {}

void Core::Event::Event::init(Core::Event::Source source) {
  this->_source = source;
}

void Core::Event::Event::reinit(void) {
  this->_source = Core::Event::Source::UNSET;
}

void Core::Event::Event::fireAsync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventAsync(this, args);
}

void Core::Event::Event::fireSync(Core::Event::IEventArgs* args) const {
  Core::Event::Manager::get().fireEventSync(this, args);
}

Core::Event::Source Core::Event::Event::getSource() const { return this->_source; }
