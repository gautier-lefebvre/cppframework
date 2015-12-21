#ifndef    __CORE_WORKER_MANAGER_HH__
#define    __CORE_WORKER_MANAGER_HH__

#include  <queue>
#include  <chrono>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Event/Event.hh"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Worker/Task.hh"
#include  "Core/Worker/Thread.hh"

namespace    Core {
  namespace  Worker {
    class  Manager :public Singleton<Core::Worker::Manager>, public Threading::Lockable, public AEndable {
      friend class Singleton<Core::Worker::Manager>;
    public:
      typedef Threading::TNotifiable<std::queue<Core::Worker::ATask*>>  TaskQueue;
      typedef Threading::TNotifiable<std::priority_queue<Core::Worker::DelayedTask*, std::vector<Core::Worker::DelayedTask*>, std::function<bool (const Core::Worker::DelayedTask*, const Worker::DelayedTask*)>>>  DelayedTaskQueue;

    private:
      TaskQueue        _pendingTasks;
      DelayedTaskQueue _delayedTasks;
      std::vector<Worker::Thread*>  _workers;

    private:
      Manager(const Manager&) = delete;
      Manager(const Manager&&) = delete;
      Manager& operator=(const Manager&) = delete;

    private:
      Manager(void);
      virtual ~Manager(void);

    public:
      virtual void end(void);

    public:
      TaskQueue&         getTaskQueue(void);
      DelayedTaskQueue&  getDelayedTaskQueue(void);

    public:
      void  init(size_t nbWorkers, bool delayedTasks);

      /**
       *  \brief Adds a Task to the task queue.
       */
      void  addTask(Core::Worker::ATask*);

      /**
       *  \brief Adds a DelayedTask to the task queue.
       */
      void  addDelayedTask(Core::Worker::DelayedTask*);

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
       */
      void  addEventTask(const Core::Event::Event*, Core::Event::IEventArgs*);

      /**
       *  \brief Adds an HTTPTask to the task queue.
       */
      void  addHTTPTask(const std::function<void (const Core::Network::HTTP::Response*)>&, const std::function<void (void)>&, Core::Network::HTTP::Response*);

      /**
       *  \brief Adds a DelayedTask to the task queue.
       */
      void  addDelayedTask(Core::Worker::ATask*, const std::chrono::steady_clock::time_point&);

      /**
       *  \brief Adds a DelayedTask to the task queue.
       */
      void  addDelayedTask(Core::Worker::ATask*, const std::chrono::steady_clock::duration&);

      /**
       *  \brief Adds a PeriodicTask to the task queue.
       */
      void  addPeriodicTask(const std::function<void(void)>&, const std::function<void(void)>&, const std::chrono::steady_clock::duration&, bool startNow = true);

      /**
       *  \brief Adds a PeriodicTask to the task queue.
       */
      void  addPeriodicTask(Core::Worker::PeriodicTask*, bool startNow = true);
    };
  }
}

#endif    /* __CORE_WORKER_MANAGER_HH__ */
