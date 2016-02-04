#ifndef    __CORE_WORKER_MANAGER_HH__
#define    __CORE_WORKER_MANAGER_HH__

#include  <queue>
#include  <chrono>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Event/Handle.hh"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Worker/Task.hh"
#include  "Core/Worker/Thread.hh"

namespace fwk {
  namespace Core {
    /**
     *  \namespace Core::Worker
     *  \brief Worker threads and tasks
     */
    namespace Worker {
      /**
       *  \class Manager Core/Worker/Manager.hh
       *  \brief Worker threads manager.
       */
      class Manager :public Singleton<fwk::Core::Worker::Manager>, public Threading::Lockable, public AEndable {
        friend class Singleton<fwk::Core::Worker::Manager>;
      public:
        typedef Threading::TNotifiable<std::queue<Core::ATask*>>  TaskQueue;
        typedef Threading::TNotifiable<std::priority_queue<Core::DelayedTask*, std::vector<Core::DelayedTask*>, std::function<bool (const Core::DelayedTask*, const Core::DelayedTask*)>>>  DelayedTaskQueue;

      private:
        TaskQueue _pendingTasks; /*!< tasks to execute. */
        DelayedTaskQueue _delayedTasks; /*!< tasks to be executed after a delay. */
        std::vector<Worker::Thread*> _workers; /*!< list of workers threads. */

      private:
        /**
         *  \brief Deleted copy constructor of Manager.
         */
        Manager(const Manager&) = delete;

        /**
         *  \brief Deleted move constructor of Manager.
         */
        Manager(const Manager&&) = delete;

        /**
         *  \brief Deleted assignment constructor of Manager.
         */
        Manager&  operator=(const Manager&) = delete;

      private:
        /**
         *  \brief Constructor of Manager.
         */
        Manager(void);

        /**
         *  \brief Destructor of Manager.
         */
        virtual ~Manager(void);

      public:
        /**
         *  \brief Stops every worker threads and clears the tasks queues.
         */
        virtual void  end(void);

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
        void  addTask(Core::ATask* task);

        /**
         *  \brief Adds a DelayedTask to the task queue.
         *  \param dtask the delayed task.
         */
        void  addDelayedTask(Core::DelayedTask* dtask);

        /**
         *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
         *  Inits it with the callback funtion.
         *  \param callback the callback of the SimpleTask object.
         */
        void  addSimpleTask(const std::function<void(void)>& callback);

        /**
         *  \brief Gets a SimpleTask from the pool and adds it to the task queue.
         *  Inits it with the callback and cleanup functions.
         *  \param callback the callback function of the SimpleTask object.
         *  \param cleanup the cleanup function of the SimpleTask object.
         */
        void  addSimpleTask(const std::function<void(void)>& callback, const std::function<void(void)>& cleanup);

        /**
         *  \brief Adds an EventTask to the task queue.
         *  \param event the event.
         *  \param args the event arguments.
         */
        void  addEventTask(const Core::Event::Handle* event, Core::Event::IEventArgs* args);

        /**
         *  \brief Adds an HTTPTask to the task queue.
         *  \param callback the function to call with the response.
         *  \param cleanup the cleanup function.
         *  \param response the HTTP response.
         */
        void  addHTTPTask(const std::function<void (const Core::Network::HTTP::Response*)>& callback, const std::function<void (void)>& cleanup, Core::Network::HTTP::Response* response);

        /**
         *  \brief Adds a DelayedTask to the delayed task queue.
         *  \param task the task to be executed at the specific time point.
         *  \param tp the timepoint at which the task must be executed.
         */
        void  addDelayedTask(Core::ATask* task, const std::chrono::steady_clock::time_point& tp);

        /**
         *  \brief Adds a DelayedTask to the delayed task queue.
         *  \param task the task to be executed after a specific duration.
         *  \param duration the duration to wait before executing the task.
         */
        void  addDelayedTask(Core::ATask* task, const std::chrono::steady_clock::duration& duration);

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
        void  addPeriodicTask(Core::PeriodicTask* periodicTask, bool startNow = true);
      };
    }
  }
}

#endif    /* __CORE_WORKER_MANAGER_HH__ */
