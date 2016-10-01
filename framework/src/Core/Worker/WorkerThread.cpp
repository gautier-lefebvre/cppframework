#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Library/Tool/Converter.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Core/Worker/WorkerThread.hh"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Event/Exception.hh"
#include  "Core/Exception.hh"

using namespace fwk;

const std::map<ATask::Source, WorkerThread::WorkerHandler> WorkerThread::TaskHandlerMap = {
    {ATask::Source::SIMPLE, &WorkerThread::executeSimpleTask},
    {ATask::Source::PERIODIC_TASK, &WorkerThread::executePeriodicTask}
};

WorkerThread::WorkerThread(size_t id):
    Lockable(),
    AEndable(),
    _id(id),
    _thread(nullptr)
{
    this->_thread = new std::thread(&WorkerThread::routine, this);
}

WorkerThread::~WorkerThread(void) {
    this->end();
}

void  WorkerThread::cleanup(void) {
    WorkerManager::TaskQueue& taskQueue = WorkerManager::get().getTaskQueue();
    WorkerManager::DelayedTaskQueue& delayedTaskQueue = WorkerManager::get().getDelayedTaskQueue();
    ATask* task;
    DelayedTask* delayedTask;
    WorkerHandler handler;

    while (!taskQueue.empty()) {
        task = WorkerManager::get().getNextTask();

        try {
            handler = WorkerThread::TaskHandlerMap.at(task->getSource());
            (*handler)(task, false);
        } catch (const std::out_of_range& e) {
            WARNING("Unknown task");
        } catch (const std::exception& e) {
            CRITICAL(e.what());
        }
    }

    while (!delayedTaskQueue.empty()) {
        delayedTask = WorkerManager::get().getNextDelayedTask();

        try {
            handler = WorkerThread::TaskHandlerMap.at(delayedTask->_task->getSource());
            (*handler)(delayedTask->_task, false);
        } catch (const std::out_of_range& e) {
            WARNING("Unknown task");
        } catch (const std::exception& e) {
            CRITICAL(e.what());
        }

        DelayedTask::returnToPool(delayedTask);
    }
}

void  WorkerThread::onEnd(void) {
    SCOPELOCK(this);
    WorkerManager::TaskQueue& taskQueue = WorkerManager::get().getTaskQueue();

    {
        ScopeLock sltasks(taskQueue);
        taskQueue.notify_all();
    }

    if (this->_thread) {
        try {
            this->_thread->join();
        } catch (const std::system_error&) {}

        delete this->_thread;
    }

    this->_thread = nullptr;
}

size_t  WorkerThread::getID(void) const {
    return this->_id;
}

void  WorkerThread::routine(void) {
    WorkerManager::TaskQueue& taskQueue = WorkerManager::get().getTaskQueue();
    ATask* task;
    WorkerHandler handler;

    while (!this->isEnding()) {
        task = nullptr;

        {
            SCOPELOCK(&taskQueue);
            if (taskQueue.empty()) {
                taskQueue.wait();
            } else {
                task = WorkerManager::get().getNextTask();
            }
        }

        if (task != nullptr) {
            try {
                handler = WorkerThread::TaskHandlerMap.at(task->getSource());
                (*handler)(task, true);
            } catch (const std::out_of_range&) {
                WARNING("Unknown task");
                delete task;
            } catch (const std::exception& e) {
                CRITICAL(e.what());
            }
        }
    }
}

void  WorkerThread::executeSimpleTask(ATask* task, bool exec) {
    SimpleTask *simpleTask = reinterpret_cast<SimpleTask*>(task);

    if (simpleTask) {
        // this boolean is used to only call the cleanup callback if it wasn't this callback that threw the exception.
        bool cleaned = false;

        try {
            try {
                if (exec) {
                    if (simpleTask->_callback) {
                        simpleTask->_callback();
                    }
                }

                if (simpleTask->_cleanup) {
                    cleaned = true;
                    simpleTask->_cleanup();
                }
            } catch (const std::exception&) {
                if (simpleTask->_cleanup && !cleaned) {
                    simpleTask->_cleanup();
                }

                throw;
            }
        } catch (const std::exception&) {
            SimpleTask::returnToPool(simpleTask);
            throw;
        }

        SimpleTask::returnToPool(simpleTask);
    } else {
        CRITICAL("Can't reinterpret_cast a SimpleTask");
    }
}

void  WorkerThread::executePeriodicTask(ATask* task, bool exec) {
    PeriodicTask *periodicTask = reinterpret_cast<PeriodicTask*>(task);

    if (periodicTask) {
        if (exec && !periodicTask->_off) {
            if (periodicTask->_callback) {
                periodicTask->_callback();
            }
        }

        // NB: not 'else' because the periodicTask could have been canceled in the callback.
        if (!exec || periodicTask->_off) {
            if (periodicTask->_cleanup) {
                periodicTask->_cleanup();
            }
            PeriodicTask::returnToPool(periodicTask);
        } else {
            WorkerManager::get().addPeriodicTask(periodicTask, false);
        }
    } else {
        CRITICAL("Can't reinterpret_cast a PeriodicTask");
    }
}
