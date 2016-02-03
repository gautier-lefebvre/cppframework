#include  "Core/Worker/Manager.hh"
#include  "Core/Exception.hh"

Core::Worker::Manager::Manager(void):
  Threading::Lockable(),
  AEndable(),
  _pendingTasks(),
  _delayedTasks([] (const Core::DelayedTask *a, const Core::DelayedTask *b) -> bool { return *a < *b; }),
  _workers()
{}

Core::Worker::Manager::~Manager(void) {
  this->end();
}

void  Core::Worker::Manager::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    this->_end = true;

    for (auto& worker : this->_workers) {
      worker->end();
      delete worker;
    }

    this->_workers.clear();
    Core::Worker::Thread::cleanup();
  }
}

void  Core::Worker::Manager::init(size_t nbTasksWorkers, bool delayedTasks) {
  SCOPELOCK(this);
  try {
    size_t i;
    for (i = 0 ; i < nbTasksWorkers ; ++i) {
      this->_workers.push_back(new Core::Worker::Thread(i, Core::Worker::Thread::Assignment::TASKS));
    }

    if (delayedTasks) {
      this->_workers.push_back(new Core::Worker::Thread(i, Core::Worker::Thread::Assignment::DELAYED_TASKS));
    }
  } catch (const std::system_error& e) {
    throw Core::Exception(e.what());
  }
}

Core::Worker::Manager::TaskQueue&      Core::Worker::Manager::getTaskQueue(void) {
  return this->_pendingTasks;
}

Core::Worker::Manager::DelayedTaskQueue&  Core::Worker::Manager::getDelayedTaskQueue(void) {
  return this->_delayedTasks;
}

void  Core::Worker::Manager::addTask(Core::ATask* task) {
  if (task != nullptr) {
    SCOPELOCK(&(this->_pendingTasks));
    this->_pendingTasks.push(task);
    this->_pendingTasks.notify();
  }
}

void  Core::Worker::Manager::addDelayedTask(Core::DelayedTask* delayedTask) {
  if (delayedTask != nullptr) {
    SCOPELOCK(&(this->_delayedTasks));
    this->_delayedTasks.push(delayedTask);
    this->_delayedTasks.notify();
  }
}

void  Core::Worker::Manager::addSimpleTask(const std::function<void (void)>& cb) {
  Core::SimpleTask *simpleTask = Core::SimpleTask::getFromPool(cb);
  this->addTask(simpleTask);
}

void  Core::Worker::Manager::addSimpleTask(const std::function<void (void)>& cb, const std::function<void (void)>& cl) {
  Core::SimpleTask *simpleTask = Core::SimpleTask::getFromPool(cb, cl);
  this->addTask(simpleTask);
}

void  Core::Worker::Manager::addEventTask(const Core::Event::Handle* event, Core::Event::IEventArgs* args) {
  if (event != nullptr) {
    Core::EventTask* eventTask = Core::EventTask::getFromPool(event, args);
    this->addTask(eventTask);
  }
}

void  Core::Worker::Manager::addHTTPTask(const std::function<void (const Core::Network::HTTP::Response*)>& cb, const std::function<void (void)>& cl, Core::Network::HTTP::Response* resp) {
  if (resp != nullptr) {
    Core::HTTPTask* httpTask = Core::HTTPTask::getFromPool(cb, cl, resp);
    this->addTask(httpTask);
  }
}

void  Core::Worker::Manager::addDelayedTask(Core::ATask* task, const std::chrono::steady_clock::time_point& timepoint) {
  if (task != nullptr) {
    Core::DelayedTask* delayedTask = Core::DelayedTask::getFromPool(task, timepoint);
    this->addDelayedTask(delayedTask);
  }
}

void  Core::Worker::Manager::addDelayedTask(Core::ATask* task, const std::chrono::steady_clock::duration& duration) {
  if (task != nullptr) {
    Core::DelayedTask* delayedTask = Core::DelayedTask::getFromPool(task, duration);
    this->addDelayedTask(delayedTask);
  }
}

void  Core::Worker::Manager::addPeriodicTask(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval, bool startNow) {
  Core::PeriodicTask* periodicTask = Core::PeriodicTask::getFromPool(callback, clean, interval);
  this->addPeriodicTask(periodicTask, startNow);
}

void  Core::Worker::Manager::addPeriodicTask(Core::PeriodicTask* periodicTask, bool startNow) {
  if (periodicTask != nullptr) {
    if (startNow) {
      this->addTask(periodicTask);
    } else {
      this->addDelayedTask(periodicTask, periodicTask->_interval);
    }
  }
}
