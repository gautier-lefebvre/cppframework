#include  "Core/Event/EventHandle.hh"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Worker/WorkerThread.hh"

using namespace fwk;

EventHandle::EventHandle(void):
  APooled<EventHandle>(),
  Lockable(),
  _subscribers()
{}

void EventHandle::reinit(void) {
  SCOPELOCK(this);
  this->_subscribers.clear();
}

void EventHandle::fireAsync(IEventArgs* args) {
  SCOPELOCK(this);
  WorkerManager::get().addEventTask(this, args);
}

void EventHandle::fireSync(IEventArgs* args) {
  EventTask* eventTask = nullptr;

  try {
    eventTask = EventTask::getFromPool(this, args);
    WorkerThread::executeEventTask(eventTask, true);
  } catch (const std::exception& e) {
    CRITICAL(e.what());
    EventTask::returnToPool(eventTask);
  }
}

void EventHandle::subscribe(const std::function<void (const IEventArgs*)>& callback, const void *key) {
  SCOPELOCK(this);
  this->_subscribers[key] = callback;
}

void EventHandle::unsubscribe(const void *key) {
  SCOPELOCK(this);
  this->_subscribers.erase(key);
}

void EventHandle::exec(const IEventArgs* args) {
  SCOPELOCK(this);

  for (auto& it : this->_subscribers) {
    try {
      if (it.second) {
        it.second(args);
      }
    } catch (const std::exception& e) {
      CRITICAL(e.what());
    }
  }
}
