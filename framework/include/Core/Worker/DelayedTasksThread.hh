#ifndef   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__
#define   __CORE_WORKER_DELAYEDTASKSTHREAD_HH__

#include  <thread>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"

namespace fwk {
  namespace Core {
    namespace Worker {
      class DelayedTasksThread :public Singleton<fwk::Core::Worker::DelayedTasksThread>, public Threading::Lockable, public AEndable {
        friend class Singleton<fwk::Core::Worker::DelayedTasksThread>;
      private:
        std::thread *_thread;

      private:
        DelayedTasksThread(const DelayedTasksThread&) = delete;
        DelayedTasksThread(const DelayedTasksThread&&) = delete;
        DelayedTasksThread& operator=(const DelayedTasksThread&) = delete;

      private:
        DelayedTasksThread(void);
        virtual ~DelayedTasksThread(void);

      public:
        virtual void  end(void);

      public:
        void  run(void);

      private:
        void  routine(void) const;
      };
    }
  }
}

#endif    /* __CORE_WORKER_DELAYEDTASKSTHREAD_HH__ */
