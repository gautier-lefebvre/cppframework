#ifndef    __CORE_WORKER_WORKERTHREAD_HH__
#define    __CORE_WORKER_WORKERTHREAD_HH__

#include  <thread>
#include  <map>

#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Notifiable.hpp"
#include  "Library/Threading/Lockable.hpp"
#include  "Core/Worker/Task.hh"

namespace fwk {
  /**
   *  \class WorkerThread Core/Worker/WorkerThread.hh
   *  \brief A thread handling tasks.
   */
  class WorkerThread :public Lockable, public AEndable {
  private:
    typedef void (*WorkerHandler)(ATask*, bool);
    static const std::map<ATask::Source, WorkerHandler> TaskHandlerMap; /*!< a map redirecting tasks to their handling methods. */

  public:
    /**
     *  \brief Cleans every tasks and clears the tasks queue.
     */
    static void cleanup();

  private:
    size_t _id; /*!< ID of the thread. */
    std::thread *_thread; /*!< thread object. */

  public:
    /**
     *  \brief Constructor of WorkerThread.
     *  \param id thread id.
     */
    WorkerThread(size_t id);

    /**
     *  \brief Destructor of WorkerThread.
     */
    virtual ~WorkerThread(void);

  public:
    /**
     *  \brief Waits until the task being executed is over, then stops the thread.
     */
    virtual void  end(void);

  public:
    /**
     *  \return the ID of the worker thread.
     */
    size_t  getID(void) const;

  public:
    /**
     *  \brief the routine for workers with TASKS assignment.
     */
    void  routine(void);

  public:
    /**
     *  \brief Executes a SimpleTask task.
     *  \throw std::exception whatever exception the SimpleTask throws.
     *  \param task pointer to the SimpleTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executeSimpleTask(ATask*, bool exec = true);

    /**
     *  \brief Executes a EventTask task.
     *  \throw std::exception whatever exception the EventTask throws.
     *  \param task pointer to the EventTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executeEventTask(ATask*, bool exec = true);

    /**
     *  \brief Executes a HttpTask task.
     *  \throw std::exception whatever exception the HttpTask throws.
     *  \param task pointer to the HttpTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executeHttpTask(ATask*, bool exec = true);

    /**
     *  \brief Executes a PeriodicTask task.
     *  \throw std::exception whatever exception the PeriodicTask throws.
     *  \param task pointer to the PeriodicTask.
     *  \param exec true if the task must be executed, or only cleaned if necessary.
     */
    static void  executePeriodicTask(ATask*, bool exec = true);
  };
}

#endif    /* __CORE_WORKER_WORKERTHREAD_HH__ */
