#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Worker/DelayedTasksThread.hh"
#include  "Core/Exception.hh"

using namespace fwk;

WorkerManager::WorkerManager(void):
  Threading::Lockable(),
  AEndable(),
  _pendingTasks(),
  _delayedTasks([] (const DelayedTask *a, const DelayedTask *b) -> bool { return *a < *b; }),
  _workers()
{}

WorkerManager::~WorkerManager(void) {
  this->end();
}

void  WorkerManager::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    this->_end = true;

    for (auto& worker : this->_workers) {
      worker->end();
      delete worker;
    }

    DelayedTasksThread::destroy();

    this->_workers.clear();
    WorkerThread::cleanup();
  }
}

void  WorkerManager::init(size_t nbTasksWorkers, bool delayedTasks) {
  SCOPELOCK(this);
  try {
    size_t i;
    for (i = 0 ; i < nbTasksWorkers ; ++i) {
      this->_workers.push_back(new WorkerThread(i));
    }

    if (delayedTasks) {
      DelayedTasksThread::get().run();
    }
  } catch (const std::system_error& e) {
    throw Exception(e.what());
  }
}

WorkerManager::TaskQueue&  WorkerManager::getTaskQueue(void) {
  return this->_pendingTasks;
}

WorkerManager::DelayedTaskQueue&  WorkerManager::getDelayedTaskQueue(void) {
  return this->_delayedTasks;
}

void  WorkerManager::addTask(ATask* task) {
  if (task != nullptr) {
    SCOPELOCK(&(this->_pendingTasks));
    this->_pendingTasks.push(task);
    this->_pendingTasks.notify();
  }
}

void  WorkerManager::addDelayedTask(DelayedTask* delayedTask) {
  if (delayedTask != nullptr) {
    SCOPELOCK(&(this->_delayedTasks));
    this->_delayedTasks.push(delayedTask);
    this->_delayedTasks.notify();
  }
}

void  WorkerManager::addSimpleTask(const std::function<void (void)>& cb) {
  SimpleTask *simpleTask = SimpleTask::getFromPool(cb);
  this->addTask(simpleTask);
}

void  WorkerManager::addSimpleTask(const std::function<void (void)>& cb, const std::function<void (void)>& cl) {
  SimpleTask *simpleTask = SimpleTask::getFromPool(cb, cl);
  this->addTask(simpleTask);
}

void  WorkerManager::addEventTask(const EventHandle* event, IEventArgs* args) {
  if (event != nullptr) {
    EventTask* eventTask = EventTask::getFromPool(event, args);
    this->addTask(eventTask);
  }
}

void  WorkerManager::addHttpTask(const std::function<void (const HttpResponse*)>& cb, const std::function<void (void)>& cl, HttpResponse* resp) {
  if (resp != nullptr) {
    HttpTask* httpTask = HttpTask::getFromPool(cb, cl, resp);
    this->addTask(httpTask);
  }
}

void  WorkerManager::addDelayedTask(ATask* task, const std::chrono::steady_clock::time_point& timepoint) {
  if (task != nullptr) {
    DelayedTask* delayedTask = DelayedTask::getFromPool(task, timepoint);
    this->addDelayedTask(delayedTask);
  }
}

void  WorkerManager::addDelayedTask(ATask* task, const std::chrono::steady_clock::duration& duration) {
  if (task != nullptr) {
    DelayedTask* delayedTask = DelayedTask::getFromPool(task, duration);
    this->addDelayedTask(delayedTask);
  }
}

void  WorkerManager::addPeriodicTask(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval, bool startNow) {
  PeriodicTask* periodicTask = PeriodicTask::getFromPool(callback, clean, interval);
  this->addPeriodicTask(periodicTask, startNow);
}

void  WorkerManager::addPeriodicTask(PeriodicTask* periodicTask, bool startNow) {
  if (periodicTask != nullptr) {
    if (startNow) {
      this->addTask(periodicTask);
    } else {
      this->addDelayedTask(periodicTask, periodicTask->_interval);
    }
  }
}
