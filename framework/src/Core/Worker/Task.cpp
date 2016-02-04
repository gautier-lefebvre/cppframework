#include  "Core/Worker/Task.hh"

using namespace fwk;

/**
 *  ATask
 */

Core::ATask::ATask(Core::ATask::Source source):
  Factory::AFactored(),
  _source(source)
{}

Core::ATask::~ATask(void) {}

Core::ATask::Source Core::ATask::getSource(void) const {
  return this->_source;
}

/**
 *  SimpleTask
 */

Core::SimpleTask::SimpleTask(void):
  Core::ATask(Core::ATask::Source::SIMPLE),
  _callback(nullptr),
  _cleanup(nullptr)
{}

Core::SimpleTask::~SimpleTask(void) {
  this->reinit();
}

void  Core::SimpleTask::reinit(void) {
  this->_callback = nullptr;
  this->_cleanup = nullptr;
}

void  Core::SimpleTask::init(const std::function<void (void)>& cb) {
  this->_callback = cb;
  this->_cleanup = nullptr;
}

void  Core::SimpleTask::init(const std::function<void (void)>& cb, const std::function<void (void)>& cl) {
  this->_callback = cb;
  this->_cleanup = cl;
}

/**
 *  EventTask
 */

Core::EventTask::EventTask(void):
  Core::ATask(Core::ATask::Source::EVENT),
  _eventCreation(),
  _event(nullptr),
  _args(nullptr)
{}

Core::EventTask::~EventTask(void) {
  this->reinit();
}

void  Core::EventTask::reinit(void) {
  this->_event = nullptr;
  this->_args = nullptr;
}

void  Core::EventTask::init(const Core::Event::Handle* ebase, Core::Event::IEventArgs* args) {
  this->_eventCreation = ebase->lastOutOfPoolTimePoint();
  this->_event = ebase;
  this->_args = args;
}

/**
 *  HTTPTask
 */

Core::HTTPTask::HTTPTask(void):
  Core::ATask(Core::ATask::Source::HTTP_CALLBACK),
  _callback(nullptr),
  _cleanup(nullptr),
  _response(nullptr)
{}

Core::HTTPTask::~HTTPTask(void) {
  this->reinit();
}

void  Core::HTTPTask::reinit(void) {
  this->_callback = nullptr;
  this->_cleanup = nullptr;
  this->_response = nullptr;
}

void  Core::HTTPTask::init(const std::function<void (const Core::Network::HTTP::Response*)>& cb, const std::function<void (void)>& cl, Core::Network::HTTP::Response* resp) {
  this->_callback = cb;
  this->_cleanup = cl;
  this->_response = resp;
}

/**
 *  PeriodicTask
 */

Core::PeriodicTask::PeriodicTask(void):
  Core::ATask(Core::ATask::Source::PERIODIC_TASK),
  _callback(nullptr),
  _clean(nullptr),
  _interval(),
  _off(true)
{}

Core::PeriodicTask::~PeriodicTask(void) {
  this->reinit();
}

void  Core::PeriodicTask::reinit(void) {
  this->_callback = nullptr;
  this->_clean = nullptr;
  this->_off = true;
}

void  Core::PeriodicTask::init(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval) {
  this->_callback = callback;
  this->_clean = clean;
  this->_interval = interval;
  this->_off = false;
}

void  Core::PeriodicTask::stop() {
  this->_off = true;
}

/**
 *  DelayedTask
 */

Core::DelayedTask::DelayedTask(void):
  Factory::AFactored(),
  _task(nullptr),
  _timePoint()
{}

Core::DelayedTask::~DelayedTask(void) {
  this->reinit();
}

void  Core::DelayedTask::reinit(void) {
  this->_task = nullptr;
}

void  Core::DelayedTask::init(Core::ATask* task, const std::chrono::steady_clock::time_point& timepoint) {
  this->_task = task;
  this->_timePoint = timepoint;
}

void  Core::DelayedTask::init(Core::ATask* task, const std::chrono::steady_clock::duration& interval) {
  this->_task = task;
  this->_timePoint = std::chrono::steady_clock::now() + interval;
}

bool  Core::DelayedTask::operator<(const Core::DelayedTask& oth) const {
  return this->_timePoint < oth._timePoint;
}

bool  Core::DelayedTask::operator>(const Core::DelayedTask& oth) const {
  return this->_timePoint > oth._timePoint;
}

bool  Core::DelayedTask::operator==(const Core::ATask *oth) const {
  return this->_task == oth;
}
