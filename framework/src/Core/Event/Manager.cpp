#include  "Library/Tool/Logger.hpp"
#include  "Core/Event/Manager.hh"
#include  "Core/Worker/Manager.hh"
#include  "Core/Worker/Thread.hh"

Core::Event::Manager::Manager(void):
  Threading::Lockable(),
  _events()
{}

Core::Event::Manager::~Manager(void) {}

void  Core::Event::Manager::registerEvent(const Core::Event::Event* event) {
  this->_events.emplace(event, Core::Event::EventInfo(event));
}

void  Core::Event::Manager::unregisterEvent(const Core::Event::Event* event) {
  this->_events.erase(event);
}

void  Core::Event::Manager::subscribeToEvent(const Core::Event::Event* event, const std::function<void (const Core::Event::IEventArgs*)>& callback, const void* callee) {
  try {
    this->_events.at(event).addSubscriber(callee, callback);
  } catch (const std::out_of_range&) {
    WARNING("Trying to subscribe to an unregistered event");
    throw Core::Event::EventNotRegisteredException();
  }
}

void  Core::Event::Manager::unsubscribeFromEvent(const Core::Event::Event* event, const void *callee) {
  try {
    this->_events.at(event).delSubscriber(callee);
  } catch (const std::out_of_range&) {
    WARNING("Trying to subscribe to an unregistered event");
    throw Core::Event::EventNotRegisteredException();
  }
}

void  Core::Event::Manager::fireEventAsync(const Core::Event::Event* event, Core::Event::IEventArgs* args) const {
  Core::Worker::Manager::get().addEventTask(event, args);
}

void  Core::Event::Manager::fireEventSync(const Core::Event::Event* event, Core::Event::IEventArgs* args) const {
  Core::EventTask* eventTask = nullptr;

  try {
    eventTask = Core::EventTask::getFromPool(event, args);
    Core::Worker::Thread::executeEventTask(eventTask, true);
  } catch (const std::exception& e) {
    CRITICAL(e.what());
    Core::EventTask::returnToPool(eventTask);
  }
}

const Core::Event::EventInfo& Core::Event::Manager::getInfo(const Core::Event::Event* event) const {
  try {
    return this->_events.at(event);
  } catch (const std::out_of_range&) {
    WARNING("Trying to retrieve the info of an unregistered event");
    throw Core::Event::EventNotRegisteredException();
  }
}
