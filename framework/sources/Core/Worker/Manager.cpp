#include  "Core/Worker/Manager.hh"
#include  "Core/Exception.hh"

Core::Worker::Manager::Manager(void):
  Threading::Lock(),
  AEndable(),
  _pendingTasks(),
  _delayedTasks([] (const Core::Worker::DelayedTask *a, const Core::Worker::DelayedTask *b) -> bool { return *a < *b; }),
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

void  Core::Worker::Manager::add(Core::Worker::ATask* task) {
  if (task != nullptr) {
    SCOPELOCK(&(this->_pendingTasks));
    this->_pendingTasks.push(task);
    this->_pendingTasks.notify();
  }
}

void  Core::Worker::Manager::add(Core::Worker::DelayedTask* delayedTask) {
  if (delayedTask != nullptr) {
    SCOPELOCK(&(this->_delayedTasks));
    this->_delayedTasks.push(delayedTask);
    this->_delayedTasks.notify();
  }
}

void  Core::Worker::Manager::add(const Core::Event::Event* event, Core::Event::IEventArgs* args) {
  if (event != nullptr) {
    Core::Worker::EventTask* eventTask = Core::Worker::EventTask::getFromPool(event, args);
    this->add(eventTask);
  }
}

void  Core::Worker::Manager::add(const std::function<void (const Core::Network::HTTP::Response*)>& cb, const std::function<void (void)>& cl, Core::Network::HTTP::Response* resp) {
  if (resp != nullptr) {
    Core::Worker::HTTPTask* httpTask = Core::Worker::HTTPTask::getFromPool(cb, cl, resp);
    this->add(httpTask);
  }
}

void  Core::Worker::Manager::add(Core::Worker::ATask* task, const std::chrono::steady_clock::time_point& timepoint) {
  if (task != nullptr) {
    Core::Worker::DelayedTask* delayedTask = Core::Worker::DelayedTask::getFromPool(task, timepoint);
    this->add(delayedTask);
  }
}

void  Core::Worker::Manager::add(Core::Worker::ATask* task, const std::chrono::steady_clock::duration& duration) {
  if (task != nullptr) {
    Core::Worker::DelayedTask* delayedTask = Core::Worker::DelayedTask::getFromPool(task, duration);
    this->add(delayedTask);
  }
}

void  Core::Worker::Manager::add(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval, bool startNow) {
  Core::Worker::PeriodicTask* periodicTask = Core::Worker::PeriodicTask::getFromPool(callback, clean, interval);
  this->add(periodicTask, startNow);
}

void  Core::Worker::Manager::add(Core::Worker::PeriodicTask* periodicTask, bool startNow) {
  if (periodicTask != nullptr) {
    if (startNow) {
      this->add(periodicTask);
    } else {
      this->add(periodicTask, periodicTask->_interval);
    }
  }
}
