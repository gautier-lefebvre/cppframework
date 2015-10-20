#include  "Core/Worker/Task.hh"

/**
 *  ATask
 */

Core::Worker::ATask::ATask(Core::Worker::ATask::Source source):
  Factory::AFactored(),
  _source(source)
{}

Core::Worker::ATask::~ATask(void) {}

Core::Worker::ATask::Source Core::Worker::ATask::getSource(void) const {
  return this->_source;
}

/**
 *  EventTask
 */

Core::Worker::EventTask::EventTask(void):
  Core::Worker::ATask(Core::Worker::ATask::Source::EVENT),
  _eventCreation(),
  _event(nullptr),
  _args(nullptr)
{}

Core::Worker::EventTask::~EventTask(void) {
  this->reinit();
}

void  Core::Worker::EventTask::reinit(void) {
  this->_event = nullptr;
  this->_args = nullptr;
}

void  Core::Worker::EventTask::init(const Core::Event::Event* ebase, Core::Event::IEventArgs* args) {
  this->_eventCreation = ebase->lastOutOfPoolTimePoint();
  this->_event = ebase;
  this->_args = args;
}

/**
 *  EventTaskPool
 */

Core::Worker::EventTask::Pool::Pool(void):
  Factory::BasicPool<Core::Worker::EventTask>()
{}

Core::Worker::EventTask::Pool::~Pool(void) {}

void  Core::Worker::EventTask::Pool::init(void) {
  this->initPool(Core::Worker::EventTask::Pool::ORIGINAL_SIZE,
    Core::Worker::EventTask::Pool::HYDRATE_SIZE,
    "Core::Worker::EventTask");
}

/**
 *  HTTPTask
 */

Core::Worker::HTTPTask::HTTPTask(void):
  Core::Worker::ATask(Core::Worker::ATask::Source::HTTP_CALLBACK)
{}

Core::Worker::HTTPTask::~HTTPTask(void) {
  this->reinit();
}

void  Core::Worker::HTTPTask::reinit(void) {
}

void  Core::Worker::HTTPTask::init(void) {
}

/**
 *  HTTPTaskPool
 */

Core::Worker::HTTPTask::Pool::Pool(void):
  Factory::BasicPool<Core::Worker::HTTPTask>()
{}

Core::Worker::HTTPTask::Pool::~Pool(void) {}

void  Core::Worker::HTTPTask::Pool::init(void) {
  this->initPool(Core::Worker::HTTPTask::Pool::ORIGINAL_SIZE,
    Core::Worker::HTTPTask::Pool::HYDRATE_SIZE,
    "Core::Worker::HTTPTask");
}

/**
 *  PeriodicTask
 */

Core::Worker::PeriodicTask::PeriodicTask(void):
  Core::Worker::ATask(Core::Worker::ATask::Source::EVENT),
  _callback(nullptr),
  _clean(nullptr),
  _interval(),
  _off(true)
{}

Core::Worker::PeriodicTask::~PeriodicTask(void) {
  this->reinit();
}

void  Core::Worker::PeriodicTask::reinit(void) {
  this->_callback = nullptr;
  this->_clean = nullptr;
  this->_off = true;
}

void  Core::Worker::PeriodicTask::init(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval) {
  this->_callback = callback;
  this->_clean = clean;
  this->_interval = interval;
  this->_off = false;
}

void  Core::Worker::PeriodicTask::stop(bool off) {
  this->_off = off;
}

/**
 *  PeriodicTaskPool
 */

Core::Worker::PeriodicTask::Pool::Pool(void):
  Factory::BasicPool<Core::Worker::PeriodicTask>()
{}

Core::Worker::PeriodicTask::Pool::~Pool(void) {}

void  Core::Worker::PeriodicTask::Pool::init(void) {
  this->initPool(Core::Worker::PeriodicTask::Pool::ORIGINAL_SIZE,
    Core::Worker::PeriodicTask::Pool::HYDRATE_SIZE,
    "Core::Worker::PeriodicTask");
}

/**
 *  DelayedTask
 */

Core::Worker::DelayedTask::DelayedTask(void):
  Factory::AFactored(),
  _task(nullptr),
  _timePoint()
{}

Core::Worker::DelayedTask::~DelayedTask(void) {
  this->reinit();
}

void  Core::Worker::DelayedTask::reinit(void) {
  this->_task = nullptr;
}

void  Core::Worker::DelayedTask::init(Core::Worker::ATask* task, const std::chrono::steady_clock::time_point& timepoint) {
  this->_task = task;
  this->_timePoint = timepoint;
}

void  Core::Worker::DelayedTask::init(Core::Worker::ATask* task, const std::chrono::steady_clock::duration& interval) {
  this->_task = task;
  this->_timePoint = std::chrono::steady_clock::now() + interval;
}

bool  Core::Worker::DelayedTask::operator<(const Core::Worker::DelayedTask& oth) const {
  return this->_timePoint < oth._timePoint;
}

bool  Core::Worker::DelayedTask::operator>(const Core::Worker::DelayedTask& oth) const {
  return this->_timePoint > oth._timePoint;
}

bool  Core::Worker::DelayedTask::operator==(const Core::Worker::ATask *oth) const {
  return this->_task == oth;
}

/**
 *  DelayedTask Pool
 */

Core::Worker::DelayedTask::Pool::Pool(void):
  Factory::BasicPool<Core::Worker::DelayedTask>()
{}

Core::Worker::DelayedTask::Pool::~Pool(void) {}

void  Core::Worker::DelayedTask::Pool::init(void) {
  this->initPool(Core::Worker::DelayedTask::Pool::ORIGINAL_SIZE,
    Core::Worker::DelayedTask::Pool::HYDRATE_SIZE,
    "Core::Worker::DelayedTask");
}
