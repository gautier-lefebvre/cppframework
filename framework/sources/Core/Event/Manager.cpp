#include	"Library/Tool/Logger.hpp"
#include	"Core/Event/Manager.hh"
#include	"Core/Worker/Manager.hh"
#include	"Core/Worker/Thread.hh"

Core::Event::Manager::Manager(void):
	Threading::Lock(),
	_events()
{}

Core::Event::Manager::~Manager(void) {}

void	Core::Event::Manager::register(const Core::Event::Event* event) {
	this->_events.emplace(event, Core::Event::EventInfo(event));
}

void	Core::Event::Manager::register(const Core::Event::Event* event, const std::function<void (Core::Event::IEventArgs*)>& cleanup) {
	this->_events.emplace(event, Core::Event::EventInfo(event, cleanup));
}

void	Core::Event::Manager::unregister(const Core::Event::Event* event) {
	this->_events.remove(event);
}

void	Core::Event::Manager::subscribe(const Core::Event::Event* event, const std::function<void (const Core::Event::IEventArgs*)>& callback, const void* callee) {
	try {
		this->_events.at(event).addSubscriber(callee, callback);
	} catch (const std::out_of_range&) {
		WARNING("Trying to subscribe to an unregistered event");
	}
}

void	Core::Event::Manager::unsubscribe(const Core::Event::Event* event, const void *callee) {
	try {
		this->_events.at(event).delSubscriber(callee);
	} catch (const std::out_of_range&) {
		WARNING("Trying to subscribe to an unregistered event");
	}
}

void	Core::Event::Manager::fireAsync(const Core::Event::Event* event, Core::Event::IEventArgs* args) const {
	Core::Worker::Manager::get().add(event, args);
}

void	Core::Event::Manager::fireSync(const Core::Event::Event* event, Core::Event::IEventArgs* args) const {
	try {
		Core::Worker::EventTask* eventTask = Core::Worker::EventTask::Pool::get().create(event, args);
		Core::Worker::Thread::executeEventTask(eventTask, true);
	} catch (const std::exception& e) {
		CRITICAL(e.what());
	}
}

const Core::Event::EventInfo& getInfo(const Core::Event::Event* event) const {
	return this->_events.at(event);
}
