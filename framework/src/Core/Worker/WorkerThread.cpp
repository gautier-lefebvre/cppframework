#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Library/Tool/Converter.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Core/Event/EventManager.hh"
#include  "Core/Worker/WorkerThread.hh"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Event/Exception.hh"
#include  "Core/Exception.hh"

using namespace fwk;

const std::map<ATask::Source, WorkerThread::WorkerHandler> WorkerThread::TaskHandlerMap = {
  {ATask::Source::SIMPLE, &WorkerThread::executeSimpleTask},
  {ATask::Source::EVENT, &WorkerThread::executeEventTask},
  {ATask::Source::HTTP_CALLBACK, &WorkerThread::executeHttpTask},
  {ATask::Source::PERIODIC_TASK, &WorkerThread::executePeriodicTask}
};

WorkerThread::WorkerThread(size_t id):
  Lockable(),
  AEndable(),
  _id(id),
  _thread(nullptr)
{
  this->_thread = new std::thread(&WorkerThread::routine, this);
}

WorkerThread::~WorkerThread(void) {
  this->end();
}

void  WorkerThread::cleanup(void) {
  WorkerManager::TaskQueue& taskQueue = WorkerManager::get().getTaskQueue();
  WorkerManager::DelayedTaskQueue& delayedTaskQueue = WorkerManager::get().getDelayedTaskQueue();
  ATask* task;
  DelayedTask* delayedTask;
  WorkerHandler handler;

  while (!taskQueue.empty()) {
    task = taskQueue.front();
    taskQueue.pop();

    try {
      handler = WorkerThread::TaskHandlerMap.at(task->getSource());
      (*handler)(task, false);
    } catch (const std::out_of_range& e) {
      WARNING("Unknown task");
    } catch (const std::exception& e) {
      CRITICAL(e.what());
    }
  }

  while (!delayedTaskQueue.empty()) {
    delayedTask = delayedTaskQueue.top();
    delayedTaskQueue.pop();

    try {
      handler = WorkerThread::TaskHandlerMap.at(delayedTask->_task->getSource());
      (*handler)(delayedTask->_task, false);
    } catch (const std::out_of_range& e) {
      WARNING("Unknown task");
    } catch (const std::exception& e) {
      CRITICAL(e.what());
    }

    DelayedTask::returnToPool(delayedTask);
  }
}

void  WorkerThread::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    WorkerManager::TaskQueue& taskQueue = WorkerManager::get().getTaskQueue();

    this->mustEnd(true);

    {
      ScopeLock sltasks(taskQueue);
      taskQueue.notify_all();
    }

    if (this->_thread) {
      try {
        this->_thread->join();
      } catch (const std::system_error&) {}

      delete this->_thread;
    }

    this->_thread = nullptr;
  }
}

size_t  WorkerThread::getID(void) const {
  return this->_id;
}

void  WorkerThread::routine(void) {
  WorkerManager::TaskQueue& taskQueue = WorkerManager::get().getTaskQueue();
  ATask* task;
  WorkerHandler handler;

  while (!this->mustEnd()) {
    task = nullptr;

    {
      SCOPELOCK(&taskQueue);
      if (taskQueue.empty()) {
        taskQueue.wait();
      } else {
        task = taskQueue.front();
        taskQueue.pop();
      }
    }

    if (task != nullptr) {
      try {
        handler = WorkerThread::TaskHandlerMap.at(task->getSource());
        (*handler)(task, true);
      } catch (const std::out_of_range&) {
        WARNING("Unknown task");
        delete task;
      } catch (const std::exception& e) {
        CRITICAL(e.what());
      }
    }
  }
}

void  WorkerThread::executeSimpleTask(ATask* task, bool exec) {
  SimpleTask *simpleTask = reinterpret_cast<SimpleTask*>(task);

  if (simpleTask) {
    try {
      if (exec) {
        if (simpleTask->_callback) {
          simpleTask->_callback();
        }
      } else {
        if (simpleTask->_cleanup) {
          simpleTask->_cleanup();
        }
      }
    } catch (const std::exception&) {
      SimpleTask::returnToPool(simpleTask);
      throw;
    }

    SimpleTask::returnToPool(simpleTask);
  } else {
    CRITICAL("Cant reinterpret_cast a SimpleTask");
  }
}

void  WorkerThread::executeEventTask(ATask* task, bool exec) {
  EventTask *eventTask = reinterpret_cast<EventTask*>(task);

  if (eventTask) {
    try {
      // execute only if the event was not unregistered before it was executed
      if (eventTask->_event->isValid() && eventTask->_eventCreation == eventTask->_event->lastOutOfPoolTimePoint()) {
        try {
          const EventInfo& eventInfo = EventManager::get().getInfo(eventTask->_event);

          if (exec) {
            for (auto& subscriber : eventInfo.subscribers) {
              subscriber.second(eventTask->_args);
            }
          }

        } catch (const EventNotRegisteredException& e) {
          WARNING(e.what());
        }
      } else if (!eventTask->_event->isValid()) {
        // if not unregistered but in the pool -> unregister now
        EventManager::get().unregisterEvent(eventTask->_event);
      }

      // return the arguments and its attributes to their factory
      if (eventTask->_args) {
        eventTask->_args->cleanup();
      }

    } catch (const std::exception&) {
      EventTask::returnToPool(eventTask);
      throw;
    }

    EventTask::returnToPool(eventTask);
  } else {
    CRITICAL("Cant reinterpret_cast an EventTask");
  }
}

void  WorkerThread::executeHttpTask(ATask* task, bool exec) {
  HttpTask *httpTask = reinterpret_cast<HttpTask*>(task);

  if (httpTask) {
    if (exec) {
      if (httpTask->_callback) {
        httpTask->_callback(httpTask->_response);
      }
    } else {
      if (httpTask->_cleanup) {
        httpTask->_cleanup();
      }
    }
    HttpResponse::returnToPool(httpTask->_response);
    HttpTask::returnToPool(httpTask);
  } else {
    CRITICAL("Cant reinterpret_cast an HTTPTask");
  }
}

void  WorkerThread::executePeriodicTask(ATask* task, bool exec) {
  PeriodicTask *periodicTask = reinterpret_cast<PeriodicTask*>(task);

  if (periodicTask) {
    if (exec == false || periodicTask->_off) {
      if (periodicTask->_cleanup) {
        periodicTask->_cleanup();
      }
      PeriodicTask::returnToPool(periodicTask);
    } else {
      periodicTask->_callback();
      WorkerManager::get().addPeriodicTask(periodicTask, false);
    }
  } else {
    CRITICAL("Cant reinterpret_cast a PeriodicTask");
  }
}
