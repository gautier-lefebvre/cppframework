#include  "Core/Event/Event.hh"

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

Core::Event::Source Core::Event::Event::getSource() const { return this->_source; }
