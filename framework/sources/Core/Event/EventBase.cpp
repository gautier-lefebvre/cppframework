#include	"Core/Event/EventBase.hh"

Core::Event::EventBase::EventBase(Core::Event::Source source):
	_source(source)
{}

Core::Event::EventBase::~EventBase() {}

Core::Event::Source Core::Event::EventBase::getSource() const { return this->_source; }
