#include	"Core/Event/Manager.hh"

Core::Event::Manager::Manager():
	Threading::Lock(),
	_events()
{}

Core::Event::Manager::~Manager() {}

void	Core::Event::Manager::register(const Core::Event::EventBase* event, const std::function<void (Core::Event::IEventArgs*)>& cleanup) {
	this->_events.emplace(event, Core::Event::EventInfo(event, cleanup));
}

void	Core::Event::Manager::unregister(const Core::Event::EventBase* event) {
	this->_events.remove(event);
}

void	Core::Event::Manager::subscribe(const Core::Event::EventBase* event, const std::function<void (const Core::Event::IEventArgs*)>& callback, const void* callee) {
	this->_events.at(event).addSubscriber(callee, callback);
}

void	Core::Event::Manager::unsubscribe(const Core::Event::EventBase* event, const void *callee) {
	this->_events.at(event).delSubscriber(callee);
}

const Core::Event::EventInfo& getInfo(const Core::Event::EventBase* event) const {
	return this->_events.at(event);
}
