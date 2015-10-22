#ifndef    __CORE_WORKER_WORKER_HH__
#define    __CORE_WORKER_WORKER_HH__

#include  <thread>
#include  <map>

#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Condition.hpp"
#include  "Library/Threading/Lock.hpp"
#include  "Core/Worker/Task.hh"

namespace  Core {
  namespace  Worker {
    class  Thread :public Threading::Lock, public AEndable {
    private:
      typedef void (*WorkerHandler)(Core::Worker::ATask*, bool);
      static const std::map<Core::Worker::ATask::Source, WorkerHandler> TaskHandlerMap;

    public:
      static void cleanup();

    public:
      enum class Assignment {
        TASKS,
        DELAYED_TASKS
      };

    private:
      size_t    _id;
      std::thread  *_thread;

    public:
      Thread(size_t, Assignment = Assignment::TASKS);
      virtual ~Thread(void);

    public:
      virtual void end(void);

    public:
      size_t  getID(void) const;

    public:
      void  tasksRoutine(void);
      void  delayedTasksRoutine(void);

    public:
      static void  executeEventTask(Core::Worker::ATask*, bool exec = true);
      static void  executeHTTPTask(Core::Worker::ATask*, bool exec = true);
      static void  executePeriodicTask(Core::Worker::ATask*, bool exec = true);
    };
  }
}

#endif    /* __CORE_WORKER_WORKER_HH__ */
