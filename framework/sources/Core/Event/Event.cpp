#include  "Core/Event/Event.hh"

Core::Event::Event::Event(Core::Event::Source source):
  _source(source)
{}

Core::Event::Event::~Event(void) {}

Core::Event::Source Core::Event::Event::getSource() const { return this->_source; }
