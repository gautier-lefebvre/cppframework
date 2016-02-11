#include  "Core/Worker/DelayedTasksThread.hh"
#include  "Core/Worker/WorkerManager.hh"

using namespace fwk;

DelayedTasksThread::DelayedTasksThread(void):
  Lockable(),
  AEndable(),
  _thread(nullptr)
{}

DelayedTasksThread::~DelayedTasksThread(void) {
  this->end();
}

void  DelayedTasksThread::run(void) {
  this->_thread = new std::thread(&DelayedTasksThread::routine, this);
}

void  DelayedTasksThread::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    WorkerManager::DelayedTaskQueue& delayedTaskQueue = WorkerManager::get().getDelayedTaskQueue();

    this->mustEnd(true);

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

void  DelayedTasksThread::routine(void) const {
  WorkerManager::DelayedTaskQueue& delayedTaskQueue = WorkerManager::get().getDelayedTaskQueue();
  DelayedTask* delayedTask;

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
      WorkerManager::get().addTask(delayedTask->_task);
      DelayedTask::returnToPool(delayedTask);
    }
  }
}
