#ifndef   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__
#define   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__

#include  <thread>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lockable.hpp"

namespace fwk {
  /**
   *  \class DelayedTasksThread Core/Worker/DelayedTasksThread.hh
   *  \brief Singleton class running on a thread and adding delayed tasks to the task queue.
   */
  class DelayedTasksThread :public Singleton<fwk::DelayedTasksThread>, public Lockable, public AEndable {
    friend class Singleton<fwk::DelayedTasksThread>;
  private:
    std::thread *_thread; /*!< the thread on which the class runs. */

  private:
    /**
     *  \brief Deleted copy constructor of DelayedTasksThread.
     */
    DelayedTasksThread(const DelayedTasksThread&) = delete;

    /**
     *  \brief Deleted move constructor of DelayedTasksThread.
     */
    DelayedTasksThread(const DelayedTasksThread&&) = delete;

    /**
     *  \brief Deleted assignment constructor of DelayedTasksThread.
     */
    DelayedTasksThread& operator=(const DelayedTasksThread&) = delete;

  private:
    /**
     *  \brief Constructor of DelayedTasksThread.
     */
    DelayedTasksThread(void);

    /**
     *  \brief Destructor of DelayedTasksThread. Ends the thread.
     */
    virtual ~DelayedTasksThread(void);

  public:
    /**
     *  \brief Notifies the thread to stop and waits for it to end.
     */
    virtual void  onEnd(void);

  public:
    /**
     *  \brief Launch the thread.
     */
    void  run(void);

  private:
    /**
     *  \brief Routine run by the thread. Adds DelayedTask objects to the task queue when the delay is over. Ends whenever the end() method is called.
     */
    void  routine(void) const;
  };
}

#endif    /* __CORE_WORKER_DELAYEDTASKSTHREAD_HH__ */
