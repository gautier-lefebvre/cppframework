#ifndef    __CORE_WORKER_WORKERMANAGER_HH__
#define    __CORE_WORKER_WORKERMANAGER_HH__

#include  <list>
#include  <unordered_map>
#include  <chrono>

#include  "Library/Collection/OrderedList.hpp"
#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lockable.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Worker/Task.hh"
#include  "Core/Worker/WorkerThread.hh"

namespace fwk {
    /**
     *  \class WorkerManager Core/Worker/WorkerManager.hh
     *  \brief Singleton class handling worker threads.
     */
    class WorkerManager :public Singleton<fwk::WorkerManager>, public Lockable, public AEndable {
        friend class Singleton<fwk::WorkerManager>;
    public:
        typedef TNotifiable<std::list<ATask*>>  TaskQueue; /*!< notifiable queue of ATask. */
        typedef TNotifiable<OrderedList<DelayedTask*>>  DelayedTaskQueue; /*!< notifiable queue of DelayedTask. */

    private:
        const std::list<TaskQueue::const_iterator> _taskQueueIteratorNullValue; /*!< this list will stay empty, this is a way to give a valid null iterator to the tasks. */
        const std::list<DelayedTaskQueue::const_iterator> _delayedTaskQueueIteratorNullValue; /*!< this list will stay empty, this is a way to give a valid null iterator to the delayed tasks. */

    private:
        TaskQueue _pendingTasks; /*!< tasks to execute. */
        DelayedTaskQueue _delayedTasks; /*!< tasks to be executed after a delay. */
        std::vector<WorkerThread*> _workers; /*!< list of workers threads. */
        bool _delayedTasksEnabled; /*!< whether or not the delayed tasks are enabled. */
        std::unordered_map<const void *, std::list<TaskQueue::const_iterator>> _watchedTasks; /*!< a map linking a key (given when creating a task) to the iterator of the task in the map, to purge. */
        std::unordered_map<const void *, std::list<DelayedTaskQueue::const_iterator>> _watchedDelayedTasks; /*!< a map linking a key (given when creating a task) to the iterator of the delayed task in the map, to purge. */

    private:
        /**
         *  \brief Deleted copy constructor of WorkerManager.
         */
        WorkerManager(const WorkerManager&) = delete;

        /**
         *  \brief Deleted move constructor of WorkerManager.
         */
        WorkerManager(const WorkerManager&&) = delete;

        /**
         *  \brief Deleted assignment constructor of WorkerManager.
         */
        WorkerManager&  operator=(const WorkerManager&) = delete;

    private:
        /**
         *  \brief Constructor of WorkerManager.
         */
        WorkerManager(void);

        /**
         *  \brief Destructor of WorkerManager.
         */
        virtual ~WorkerManager(void);

    public:
        /**
         *  \brief Stops every worker threads and clears the tasks queues.
         */
        virtual void  onEnd(void);

    public:
        /**
         *  \return the tasks queue.
         */
        TaskQueue&  getTaskQueue(void);

        /**
         *  \return the delayed tasks queue.
         */
        DelayedTaskQueue&  getDelayedTaskQueue(void);

        /**
         *  \brief Pops the first task from the task queue and returns it. The caller should lock the task queue beforehand.
         */
        ATask* getNextTask(void);

        /**
         *  \brief Pops the first delayed task from the delayed task queue and returns it. The caller should lock the delayed task queue beforehand.
         */
        DelayedTask* getNextDelayedTask(void);

    public:
        /**
         *  \brief Initializes the worker threads.
         *  \param nbWorkers the number of workers executing the tasks of the task queue.
         *  \param delayedTasks true if a thread must be used for delayed tasks.
         */
        void  init(size_t nbWorkers, bool delayedTasks);

        /**
         *  \brief Adds a Task to the task queue.
         *  \param task the task.
         */
        void  addTask(ATask* task);

        /**
         *  \brief Adds a DelayedTask to the task queue.
         *  \param dtask the delayed task.
         */
        void  addDelayedTask(DelayedTask* dtask);

        /**
         *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
         *  Inits it with the callback function.
         *  \param key the key used to purge the task queue.
         *  \param callback the callback of the SimpleTask object.
         */
        void  addSimpleTask(const void* key, const std::function<void (void)>& callback);

        /**
         *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
         *  Inits it with the callback and cleanup functions.
         *  \param key the key used to purge the task queue.
         *  \param callback the callback function of the SimpleTask object.
         *  \param cleanup the cleanup function of the SimpleTask object.
         */
        void  addSimpleTask(const void* key, const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);

        /**
         *  \brief Adds a DelayedTask to the delayed task queue.
         *  \param task the task to be executed at the specific time point.
         *  \param tp the timepoint at which the task must be executed.
         */
        void  addDelayedTask(ATask* task, const std::chrono::steady_clock::time_point& tp);

        /**
         *  \brief Adds a DelayedTask to the delayed task queue.
         *  \param task the task to be executed after a specific duration.
         *  \param duration the duration to wait before executing the task.
         */
        void  addDelayedTask(ATask* task, const std::chrono::steady_clock::duration& duration);

        /**
         *  \brief Adds a PeriodicTask to the task queue.
         *  \param key the key used to purge the task queue.
         *  \param callback the function to call at regular interval.
         *  \param cleanup the function to call after the periodic task has been canceled.
         *  \param duration the duration between 2 calls of the callback.
         *  \param startNow true if the task must be first executed right away, false if the task must be first executed after the specified duration.
         */
        void  addPeriodicTask(const void* key, const std::function<void(void)>& callback, const std::function<void(void)>& cleanup, const std::chrono::steady_clock::duration& duration, bool startNow = true);

        /**
         *  \brief Adds a PeriodicTask to the task queue.
         *  \param periodicTask the PeriodicTask.
         *  \param startNow true if the task must be first executed right away, false if the task must be first executed after the specified duration.
         */
        void  addPeriodicTask(PeriodicTask* periodicTask, bool startNow = true);

    public:
        /**
         *  \brief Removes all Tasks added with the given key.
         *  \param key the key given in addSimpleTask.
         */
        void purgeTaskQueue(const void *key);
    };
}

#endif    /* __CORE_WORKER_WORKERMANAGER_HH__ */
