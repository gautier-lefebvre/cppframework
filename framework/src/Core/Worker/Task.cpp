#include  "Core/Worker/Task.hh"

using namespace fwk;

/**
 *  ATask
 */

ATask::ATask(ATask::Source source):
    _source(source),
    _key(nullptr)
{}

ATask::~ATask(void) {}

ATask::Source ATask::getSource(void) const {
    return this->_source;
}

const void* ATask::getKey(void) const {
    return this->_key;
}

/**
 *  SimpleTask
 */

SimpleTask::SimpleTask(void):
    ATask(ATask::Source::SIMPLE),
    APooled<SimpleTask>(),
    _callback(nullptr),
    _cleanup(nullptr)
{}

SimpleTask::~SimpleTask(void) {
    this->reinit();
}

void  SimpleTask::reinit(void) {
    this->_key = nullptr;
    this->_callback = nullptr;
    this->_cleanup = nullptr;
}

void  SimpleTask::init(const void* key, const std::function<void (void)>& cb) {
    this->_key = key;
    this->_callback = cb;
    this->_cleanup = nullptr;
}

void  SimpleTask::init(const void* key, const std::function<void (void)>& cb, const std::function<void (void)>& cl) {
    this->_key = key;
    this->_callback = cb;
    this->_cleanup = cl;
}

/**
 *  HttpTask
 */

HttpTask::HttpTask(void):
    ATask(ATask::Source::HTTP_CALLBACK),
    APooled<HttpTask>(),
    _callback(nullptr),
    _cleanup(nullptr),
    _response(nullptr)
{}

HttpTask::~HttpTask(void) {
    this->reinit();
}

void  HttpTask::reinit(void) {
    this->_callback = nullptr;
    this->_cleanup = nullptr;
    this->_response = nullptr;
}

void  HttpTask::init(const std::function<void (const HttpResponse*)>& cb, const std::function<void (void)>& cl, HttpResponse* resp) {
    this->_callback = cb;
    this->_cleanup = cl;
    this->_response = resp;
}

/**
 *  PeriodicTask
 */

PeriodicTask::PeriodicTask(void):
    ATask(ATask::Source::PERIODIC_TASK),
    APooled<PeriodicTask>(),
    _callback(nullptr),
    _cleanup(nullptr),
    _interval(),
    _off(true)
{}

PeriodicTask::~PeriodicTask(void) {
    this->reinit();
}

void  PeriodicTask::reinit(void) {
    this->_key = nullptr;
    this->_callback = nullptr;
    this->_cleanup = nullptr;
    this->_off = true;
}

void  PeriodicTask::init(const void* key, const std::function<void(void)>& callback, const std::function<void(void)>& cleanup, const std::chrono::steady_clock::duration& interval) {
    this->_key = key;
    this->_callback = callback;
    this->_cleanup = cleanup;
    this->_interval = interval;
    this->_off = false;
}

void  PeriodicTask::stop() {
    this->_off = true;
}

/**
 *  DelayedTask
 */

DelayedTask::DelayedTask(void):
    APooled<DelayedTask>(),
    _task(nullptr),
    _timePoint()
{}

DelayedTask::~DelayedTask(void) {
    this->reinit();
}

void  DelayedTask::reinit(void) {
    this->_task = nullptr;
}

void  DelayedTask::init(ATask* task, const std::chrono::steady_clock::time_point& timepoint) {
    this->_task = task;
    this->_timePoint = timepoint;
}

void  DelayedTask::init(ATask* task, const std::chrono::steady_clock::duration& interval) {
    this->_task = task;
    this->_timePoint = std::chrono::steady_clock::now() + interval;
}

bool  DelayedTask::operator<(const DelayedTask& oth) const {
    return this->_timePoint < oth._timePoint;
}

bool  DelayedTask::operator>(const DelayedTask& oth) const {
    return this->_timePoint > oth._timePoint;
}

bool  DelayedTask::operator==(const ATask *oth) const {
    return this->_task == oth;
}
