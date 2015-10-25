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
    class  Manager :public Singleton<Core::Worker::Manager>, public Threading::Lock, public AEndable {
      friend class Singleton<Core::Worker::Manager>;
    public:
      typedef Threading::Notifiable<std::queue<Core::Worker::ATask*>>  TaskQueue;
      typedef Threading::Notifiable<std::priority_queue<Core::Worker::DelayedTask*, std::vector<Core::Worker::DelayedTask*>, std::function<bool (const Core::Worker::DelayedTask*, const Worker::DelayedTask*)>>>  DelayedTaskQueue;

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
      void  add(Core::Worker::ATask*);
      void  add(Core::Worker::DelayedTask*);
      void  add(const Core::Event::Event*, Core::Event::IEventArgs*);
      void  add(const std::function<void (const Core::Network::HTTP::Response*)>&, const std::function<void (void)>&, Core::Network::HTTP::Response*);
      void  add(Core::Worker::ATask*, const std::chrono::steady_clock::time_point&);
      void  add(Core::Worker::ATask*, const std::chrono::steady_clock::duration&);
      void  add(const std::function<void(void)>&, const std::function<void(void)>&, const std::chrono::steady_clock::duration&, bool startNow = true);
      void  add(Core::Worker::PeriodicTask*, bool startNow = true);
    };
  }
}

#endif    /* __CORE_WORKER_MANAGER_HH__ */
