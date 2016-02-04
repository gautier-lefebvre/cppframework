#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Library/Tool/Converter.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Core/Event/Manager.hh"
#include  "Core/Worker/Thread.hh"
#include  "Core/Worker/Manager.hh"
#include  "Core/Event/Exception.hh"
#include  "Core/Exception.hh"

using namespace fwk;

const std::map<Core::ATask::Source, Core::Worker::Thread::WorkerHandler> Core::Worker::Thread::TaskHandlerMap = {
  {Core::ATask::Source::SIMPLE, &Core::Worker::Thread::executeSimpleTask},
  {Core::ATask::Source::EVENT, &Core::Worker::Thread::executeEventTask},
  {Core::ATask::Source::HTTP_CALLBACK, &Core::Worker::Thread::executeHTTPTask},
  {Core::ATask::Source::PERIODIC_TASK, &Core::Worker::Thread::executePeriodicTask}
};

Core::Worker::Thread::Thread(size_t id, Core::Worker::Thread::Assignment assignment):
  Threading::Lockable(),
  AEndable(),
  _id(id),
  _thread(nullptr)
{
  void  (Core::Worker::Thread::*routine)() = nullptr;
  if (assignment == Core::Worker::Thread::Assignment::TASKS) {
    routine = &Core::Worker::Thread::tasksRoutine;
  } else {
    routine = &Core::Worker::Thread::delayedTasksRoutine;
  }

  if (routine != nullptr) {
    this->_thread = new std::thread(routine, this);
  } else {
    throw Core::Exception(fmt::format("Invalid assignment for worker {0}", this->_id));
  }
}

Core::Worker::Thread::~Thread(void) {
  this->end();
}

void  Core::Worker::Thread::cleanup(void) {
  Core::Worker::Manager::TaskQueue& taskQueue = Core::Worker::Manager::get().getTaskQueue();
  Core::Worker::Manager::DelayedTaskQueue& delayedTaskQueue = Core::Worker::Manager::get().getDelayedTaskQueue();
  Core::ATask* task;
  Core::DelayedTask* delayedTask;
  WorkerHandler handler;

  while (!taskQueue.empty()) {
    task = taskQueue.front();
    taskQueue.pop();

    try {
      handler = Core::Worker::Thread::TaskHandlerMap.at(task->getSource());
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
      handler = Core::Worker::Thread::TaskHandlerMap.at(delayedTask->_task->getSource());
      (*handler)(delayedTask->_task, false);
    } catch (const std::out_of_range& e) {
      WARNING("Unknown task");
    } catch (const std::exception& e) {
      CRITICAL(e.what());
    }

    Core::DelayedTask::returnToPool(delayedTask);
  }
}

void  Core::Worker::Thread::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    Core::Worker::Manager::TaskQueue& taskQueue = Core::Worker::Manager::get().getTaskQueue();
    Core::Worker::Manager::DelayedTaskQueue& delayedTaskQueue = Core::Worker::Manager::get().getDelayedTaskQueue();

    this->mustEnd(true);

    {
      ScopeLock sltasks(taskQueue);
      taskQueue.notify_all();
    }

    {
      ScopeLock sldtasks(delayedTaskQueue);
      delayedTaskQueue.notify_all();
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

size_t  Core::Worker::Thread::getID(void) const {
  return this->_id;
}

void  Core::Worker::Thread::tasksRoutine(void) {
  Core::Worker::Manager::TaskQueue& taskQueue = Core::Worker::Manager::get().getTaskQueue();
  Core::ATask* task;
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
        handler = Core::Worker::Thread::TaskHandlerMap.at(task->getSource());
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

void  Core::Worker::Thread::executeSimpleTask(Core::ATask* task, bool exec) {
  Core::SimpleTask *simpleTask = reinterpret_cast<Core::SimpleTask*>(task);

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
    } catch (const std::exception& e) {
      Core::SimpleTask::returnToPool(simpleTask);
      throw e;
    }

    Core::SimpleTask::returnToPool(simpleTask);
  } else {
    CRITICAL("Cant reinterpret_cast a SimpleTask");
  }
}

void  Core::Worker::Thread::executeEventTask(Core::ATask* task, bool exec) {
  Core::EventTask *eventTask = reinterpret_cast<Core::EventTask*>(task);

  if (eventTask) {
    try {
      // execute only if the event was not unregistered before it was executed
      if (eventTask->_event->isValid() && eventTask->_eventCreation == eventTask->_event->lastOutOfPoolTimePoint()) {
        try {
          const Core::Event::EventInfo& eventInfo = Core::Event::Manager::get().getInfo(eventTask->_event);

          if (exec) {
            for (auto& subscriber : eventInfo.subscribers) {
              subscriber.second(eventTask->_args);
            }
          }

        } catch (const Core::Event::EventNotRegisteredException& e) {
          WARNING(e.what());
        }
      }
      // if not unregistered but in the pool -> unregister now
      else if (!eventTask->_event->isValid()) {
        Core::Event::Manager::get().unregisterEvent(eventTask->_event);
      }

      // return the arguments and its attributes to their factory
      if (eventTask->_args) {
        eventTask->_args->cleanup();
      }

    } catch (const std::exception& e) {
      Core::EventTask::returnToPool(eventTask);
      throw e;
    }

    Core::EventTask::returnToPool(eventTask);
  } else {
    CRITICAL("Cant reinterpret_cast an EventTask");
  }
}

void  Core::Worker::Thread::executeHTTPTask(Core::ATask* task, bool exec) {
  Core::HTTPTask *httpTask = reinterpret_cast<Core::HTTPTask*>(task);

  if (httpTask) {
    if (exec) {
      if (httpTask->_callback) {
        httpTask->_callback(httpTask->_response);
      }
    }
    if (httpTask->_cleanup) {
      httpTask->_cleanup();
    }
    Core::Network::HTTP::Response::returnToPool(httpTask->_response);
    Core::HTTPTask::returnToPool(httpTask);
  } else {
    CRITICAL("Cant reinterpret_cast an HTTPTask");
  }
}

void  Core::Worker::Thread::executePeriodicTask(Core::ATask* task, bool exec) {
  Core::PeriodicTask *periodicTask = reinterpret_cast<Core::PeriodicTask*>(task);

  if (periodicTask) {
    if (exec == false || periodicTask->_off) {
      if (periodicTask->_clean) {
        periodicTask->_clean();
      }
      Core::PeriodicTask::returnToPool(periodicTask);
    } else {
      periodicTask->_callback();
      Core::Worker::Manager::get().addPeriodicTask(periodicTask, false);
    }
  } else {
    CRITICAL("Cant reinterpret_cast a PeriodicTask");
  }
}

void  Core::Worker::Thread::delayedTasksRoutine(void) {
  Core::Worker::Manager::DelayedTaskQueue& delayedTaskQueue = Core::Worker::Manager::get().getDelayedTaskQueue();
  Core::DelayedTask* delayedTask;

  while (!this->mustEnd()) {
    delayedTask = nullptr;

    {
      SCOPELOCK(&delayedTaskQueue);
      if (delayedTaskQueue.empty()) {
        delayedTaskQueue.wait();
      } else {
        if (delayedTaskQueue.wait_until(delayedTaskQueue.top()->_timePoint) == std::cv_status::timeout) {
          if (this->mustEnd()) { break; }
          if (!delayedTaskQueue.empty()) {
            if ((delayedTask = delayedTaskQueue.top())->_timePoint <= std::chrono::steady_clock::now()) {
              delayedTaskQueue.pop();
            } else {
              delayedTask = nullptr;
            }
          }
        }
      }
    }

    if (delayedTask != nullptr) {
      Core::Worker::Manager::get().addTask(delayedTask->_task);
      Core::DelayedTask::returnToPool(delayedTask);
    }
  }
}
