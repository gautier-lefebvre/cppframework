#include  <iterator>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Worker/DelayedTasksThread.hh"
#include  "Core/Exception.hh"

using namespace fwk;

WorkerManager::WorkerManager(void):
    Lockable(),
    AEndable(),
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

    if (task->getKey() && task->_taskIterator != this->_watchedTasks.cend()) {
        this->_watchedTasks.erase(task->_taskIterator);
    }

    return task;
}

DelayedTask* WorkerManager::getNextDelayedTask(void) {
    if (this->_delayedTasks.empty()) { return nullptr; }

    DelayedTask* delayedTask = this->_delayedTasks.front();
    this->_delayedTasks.pop_front();

    if (delayedTask->_task->getKey() && delayedTask->_delayedTaskIterator != this->_watchedDelayedTasks.cend()) {
        this->_watchedDelayedTasks.erase(delayedTask->_delayedTaskIterator);
    }

    return delayedTask;
}

void  WorkerManager::addTask(ATask* task) {
    if (task != nullptr) {
        SCOPELOCK(&(this->_pendingTasks));
        this->_pendingTasks.push_back(task);

        if (task->getKey()) {
            task->_taskIterator = this->_watchedTasks.emplace(task->getKey(), std::prev(this->_pendingTasks.cend()));
        } else {
            task->_taskIterator = this->_watchedTasks.cend();
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
                delayedTask->_delayedTaskIterator = this->_watchedDelayedTasks.emplace(delayedTask->_task->getKey(), std::prev(this->_delayedTasks.cend()));
            } else {
                delayedTask->_delayedTaskIterator = this->_watchedDelayedTasks.cend();
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
    {
        SCOPELOCK(&(this->_pendingTasks));

        while ((auto it = this->_watchedTasks.find(key)) != this->_watchedTasks.end()) {
            ATask* task = *((*it).second);
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

            this->_pendingTasks.erase((*it).second);
            this->_watchedTasks.erase(it);
        }

        this->_pendingTasks.notify_all();
    }

    {
        SCOPELOCK(&(this->_delayedTasks));

        while ((auto it = this->_watchedDelayedTasks.find(key)) != this->_watchedDelayedTasks.end()) {
            ATask* task = (*((*it).second))->_task;
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

            DelayedTask::returnToPool(*((*it).second));
            this->_delayedTasks.erase((*it).second);
            this->_watchedDelayedTasks.erase(it);
        }

        this->_delayedTasks.notify_all();
    }
}
