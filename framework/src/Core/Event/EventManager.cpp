#include  "Library/Tool/Logger.hpp"
#include  "Core/Event/EventManager.hh"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Worker/WorkerThread.hh"

using namespace fwk;

EventManager::EventManager(void):
  Lockable(),
  _events()
{}

EventManager::~EventManager(void) {}

void  EventManager::registerEvent(const EventHandle* event) {
  this->_events.emplace(event, EventInfo(event));
}

void  EventManager::unregisterEvent(const EventHandle* event) {
  this->_events.erase(event);
}

void  EventManager::subscribeToEvent(const EventHandle* event, const std::function<void (const IEventArgs*)>& callback, const void* callee) {
  try {
    this->_events.at(event).addSubscriber(callee, callback);
  } catch (const std::out_of_range&) {
    WARNING("Trying to subscribe to an unregistered event");
    throw EventNotRegisteredException();
  }
}

void  EventManager::unsubscribeFromEvent(const EventHandle* event, const void *callee) {
  try {
    this->_events.at(event).delSubscriber(callee);
  } catch (const std::out_of_range&) {
    WARNING("Trying to subscribe to an unregistered event");
    throw EventNotRegisteredException();
  }
}

void  EventManager::fireEventAsync(const EventHandle* event, IEventArgs* args) const {
  WorkerManager::get().addEventTask(event, args);
}

void  EventManager::fireEventSync(const EventHandle* event, IEventArgs* args) const {
  EventTask* eventTask = nullptr;

  try {
    eventTask = EventTask::getFromPool(event, args);
    WorkerThread::executeEventTask(eventTask, true);
  } catch (const std::exception& e) {
    CRITICAL(e.what());
    EventTask::returnToPool(eventTask);
  }
}

const EventInfo& EventManager::getInfo(const EventHandle* event) const {
  try {
    return this->_events.at(event);
  } catch (const std::out_of_range&) {
    WARNING("Trying to retrieve the info of an unregistered event");
    throw EventNotRegisteredException();
  }
}
