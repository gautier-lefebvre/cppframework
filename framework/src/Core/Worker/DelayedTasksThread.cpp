#include  "Core/Worker/DelayedTasksThread.hh"
#include  "Core/Worker/Manager.hh"

using namespace fwk;

Core::Worker::DelayedTasksThread::DelayedTasksThread(void):
  Threading::Lockable(),
  AEndable(),
  _thread(nullptr)
{}

Core::Worker::DelayedTasksThread::~DelayedTasksThread(void) {
  this->end();
}

void  Core::Worker::DelayedTasksThread::run(void) {
  this->_thread = new std::thread(&Core::Worker::DelayedTasksThread::routine, this);
}

void  Core::Worker::DelayedTasksThread::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    Core::Worker::Manager::DelayedTaskQueue& delayedTaskQueue = Core::Worker::Manager::get().getDelayedTaskQueue();

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

void  Core::Worker::DelayedTasksThread::routine(void) const {
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
