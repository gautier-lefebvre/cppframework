#ifndef    __CORE_WORKER_WORKERMANAGER_HH__
#define    __CORE_WORKER_WORKERMANAGER_HH__

#include  <queue>
#include  <chrono>

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
    typedef TNotifiable<std::queue<ATask*>>  TaskQueue; /*!< notifiable queue of ATask. */
    typedef TNotifiable<std::priority_queue<DelayedTask*, std::vector<DelayedTask*>, std::function<bool (const DelayedTask*, const DelayedTask*)>>>  DelayedTaskQueue; /*!< notifiable queue of DelayedTask. */

  private:
    TaskQueue _pendingTasks; /*!< tasks to execute. */
    DelayedTaskQueue _delayedTasks; /*!< tasks to be executed after a delay. */
    std::vector<WorkerThread*> _workers; /*!< list of workers threads. */
    bool _delayedTasksEnabled; /*!< wether or not the delayed tasks are enabled. */

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
     *  Inits it with the callback funtion.
     *  \param callback the callback of the SimpleTask object.
     */
    void  addSimpleTask(const std::function<void (void)>& callback);

    /**
     *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
     *  Inits it with the callback and cleanup functions.
     *  \param callback the callback function of the SimpleTask object.
     *  \param cleanup the cleanup function of the SimpleTask object.
     */
    void  addSimpleTask(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);

    /**
     *  \brief Adds an EventTask to the task queue.
     *  \param eventTimePoint the time point when the event was taken out of the pool. This parameter is only used to determine whether or not the event must be executed when it is fired (in the case of an asynchronous event).
     *  \param callback the function to call when the event is fired.
     */
    void  addEventTask(const std::chrono::steady_clock::time_point& eventTimePoint, const std::function<void (const std::chrono::steady_clock::time_point&)>& callback);

    /**
     *  \brief Adds an HttpTask to the task queue.
     *  \param callback the function to call with the response.
     *  \param cleanup the cleanup function.
     *  \param response the HTTP response.
     */
    void  addHttpTask(const std::function<void (const HttpResponse*)>& callback, const std::function<void (void)>& cleanup, HttpResponse* response);

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
     *  \param callback the function to call at regular interval.
     *  \param cleanup the function to call after the periodic task has been canceled.
     *  \param duration the duration between 2 calls of the callback.
     *  \param startNow true if the task must be first executed right away, false if the task must be first executed after the specified duration.
     */
    void  addPeriodicTask(const std::function<void(void)>& callback, const std::function<void(void)>& cleanup, const std::chrono::steady_clock::duration& duration, bool startNow = true);

    /**
     *  \brief Adds a PeriodicTask to the task queue.
     *  \param periodicTask the PeriodicTask.
     *  \param startNow true if the task must be first executed right away, false if the task must be first executed after the specified duration.
     */
    void  addPeriodicTask(PeriodicTask* periodicTask, bool startNow = true);
  };
}

#endif    /* __CORE_WORKER_WORKERMANAGER_HH__ */
