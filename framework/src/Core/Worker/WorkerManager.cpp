#include  <iterator>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Worker/DelayedTasksThread.hh"
#include  "Core/Exception.hh"

using namespace fwk;

WorkerManager::WorkerManager(void):
    Lockable(),
    AEndable(),
    _taskQueueIteratorNullValue(),
    _delayedTaskQueueIteratorNullValue(),
    _pendingTasks(),
    _delayedTasks([] (const DelayedTask *a, const DelayedTask *b) -> bool { return *a < *b; }),
    _workers(),
    _delayedTasksEnabled(false),
    _watchedTasks(),
    _watchedDelayedTasks()
{}

WorkerManager::~WorkerManager(void) {
    this->end();
}

void  WorkerManager::onEnd(void) {
    SCOPELOCK(this);

    for (auto& worker : this->_workers) {
        worker->end();
        delete worker;
    }

    DelayedTasksThread::destroy();

    this->_workers.clear();
    WorkerThread::cleanup();
}

void  WorkerManager::init(size_t nbTasksWorkers, bool delayedTasks) {
    SCOPELOCK(this);
    try {
        size_t i;
        for (i = 0 ; i < nbTasksWorkers ; ++i) {
            this->_workers.push_back(new WorkerThread(i));
        }

        this->_delayedTasksEnabled = delayedTasks;

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

ATask* WorkerManager::getNextTask(void) {
    if (this->_pendingTasks.empty()) { return nullptr; }

    ATask* task = this->_pendingTasks.front();
    this->_pendingTasks.pop_front();

    if (task->getKey() && task->_taskIterator != this->_taskQueueIteratorNullValue.cend()) {
        try {
            this->_watchedTasks.at(task->getKey()).erase(task->_taskIterator);
        } catch (const std::out_of_range&) {}
    }

    return task;
}

DelayedTask* WorkerManager::getNextDelayedTask(void) {
    if (this->_delayedTasks.empty()) { return nullptr; }

    DelayedTask* delayedTask = this->_delayedTasks.front();
    this->_delayedTasks.pop_front();

    if (delayedTask->_task->getKey() && delayedTask->_delayedTaskIterator != this->_delayedTaskQueueIteratorNullValue.cend()) {
        try {
            this->_watchedDelayedTasks.at(delayedTask->_task->getKey()).erase(delayedTask->_delayedTaskIterator);
        } catch (const std::out_of_range&) {}
    }

    return delayedTask;
}

void  WorkerManager::addTask(ATask* task) {
    if (task != nullptr) {
        SCOPELOCK(&(this->_pendingTasks));
        this->_pendingTasks.push_back(task);

        if (task->getKey()) {
            auto& iterators = this->_watchedTasks[task->getKey()];
            iterators.push_back(std::prev(this->_pendingTasks.cend()));
            task->_taskIterator = std::prev(iterators.cend());
        } else {
            task->_taskIterator = this->_taskQueueIteratorNullValue.cend();
        }

        this->_pendingTasks.notify();
    }
}

void  WorkerManager::addDelayedTask(DelayedTask* delayedTask) {
    if (this->_delayedTasksEnabled) {
        if (delayedTask != nullptr) {
            SCOPELOCK(&(this->_delayedTasks));
            this->_delayedTasks.push(delayedTask);

            if (delayedTask->_task->getKey()) {
                auto& iterators = this->_watchedDelayedTasks[delayedTask->_task->getKey()];
                iterators.push_back(std::prev(this->_delayedTasks.cend()));
                delayedTask->_delayedTaskIterator = std::prev(iterators.cend());
            } else {
                delayedTask->_delayedTaskIterator = this->_delayedTaskQueueIteratorNullValue.cend();
            }

            this->_delayedTasks.notify();
        }
    } else {
        DelayedTask::returnToPool(delayedTask);
        WARNING("Delayed tasks are disabled.");
    }
}

void  WorkerManager::addSimpleTask(const void* key, const std::function<void (void)>& cb) {
    SimpleTask *simpleTask = SimpleTask::getFromPool(key, cb);
    this->addTask(simpleTask);
}

void  WorkerManager::addSimpleTask(const void* key, const std::function<void (void)>& cb, const std::function<void (void)>& cl) {
    SimpleTask *simpleTask = SimpleTask::getFromPool(key, cb, cl);
    this->addTask(simpleTask);
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

void  WorkerManager::addPeriodicTask(const void* key, const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval, bool startNow) {
    PeriodicTask* periodicTask = PeriodicTask::getFromPool(key, callback, clean, interval);
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

void WorkerManager::purgeTaskQueue(const void* key) {
    try {
        SCOPELOCK(&(this->_pendingTasks));

        for (auto& iterator : this->_watchedTasks.at(key)) {
            ATask* task = *iterator;
            switch (task->getSource()) {
                case ATask::Source::SIMPLE: {
                    SimpleTask* simpleTask = reinterpret_cast<SimpleTask*>(task);
                    SimpleTask::returnToPool(simpleTask);
                    break;
                }
                case ATask::Source::PERIODIC_TASK: {
                    PeriodicTask* periodicTask = reinterpret_cast<PeriodicTask*>(task);
                    PeriodicTask::returnToPool(periodicTask);
                    break;
                }
            }

            this->_pendingTasks.erase(iterator);
        }

        this->_watchedTasks.erase(key);
        this->_pendingTasks.notify_all();
    } catch (const std::out_of_range&) {}

    try {
        SCOPELOCK(&(this->_delayedTasks));

        for (auto& iterator : this->_watchedDelayedTasks.at(key)) {
            ATask* task = (*iterator)->_task;
            switch (task->getSource()) {
                case ATask::Source::SIMPLE: {
                    SimpleTask* simpleTask = reinterpret_cast<SimpleTask*>(task);
                    SimpleTask::returnToPool(simpleTask);
                    break;
                }
                case ATask::Source::PERIODIC_TASK: {
                    PeriodicTask* periodicTask = reinterpret_cast<PeriodicTask*>(task);
                    PeriodicTask::returnToPool(periodicTask);
                    break;
                }
            }

            DelayedTask::returnToPool(*iterator);
            this->_delayedTasks.erase(iterator);
        }

        this->_watchedDelayedTasks.erase(key);
        this->_delayedTasks.notify_all();
    } catch (const std::out_of_range&) {}
}
