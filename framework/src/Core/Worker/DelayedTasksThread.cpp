#include  "Core/Worker/DelayedTasksThread.hh"
#include  "Core/Worker/WorkerManager.hh"

using namespace fwk;

DelayedTasksThread::DelayedTasksThread(void):
    Lockable(),
    AEndable(),
    _thread(nullptr)
{}

DelayedTasksThread::~DelayedTasksThread(void) {
    this->end();
}

void  DelayedTasksThread::run(void) {
    this->_thread = new std::thread(&DelayedTasksThread::routine, this);
}

void  DelayedTasksThread::onEnd(void) {
    SCOPELOCK(this);
    WorkerManager::DelayedTaskQueue& delayedTaskQueue = WorkerManager::get().getDelayedTaskQueue();

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

void  DelayedTasksThread::routine(void) const {
    WorkerManager::DelayedTaskQueue& delayedTaskQueue = WorkerManager::get().getDelayedTaskQueue();
    DelayedTask* delayedTask;

    while (!this->isEnding()) {
        delayedTask = nullptr;

        {
            SCOPELOCK(&delayedTaskQueue);
            if (delayedTaskQueue.empty()) {
                delayedTaskQueue.wait();
            } else {
                if (delayedTaskQueue.wait_until(delayedTaskQueue.front()->_timePoint) == std::cv_status::timeout) {
                    if (this->isEnding()) { break; }
                    if (!delayedTaskQueue.empty()) {
                        if (delayedTaskQueue.front()->_timePoint <= std::chrono::steady_clock::now()) {
                            delayedTask = WorkerManager::get().getNextDelayedTask();
                        } else {
                            delayedTask = nullptr;
                        }
                    }
                }
            }
        }

        if (delayedTask != nullptr) {
            WorkerManager::get().addTask(delayedTask->_task);
            DelayedTask::returnToPool(delayedTask);
        }
    }
}
