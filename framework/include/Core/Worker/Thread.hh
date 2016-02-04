#ifndef    __CORE_WORKER_WORKER_HH__
#define    __CORE_WORKER_WORKER_HH__

#include  <thread>
#include  <map>

#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Condition.hpp"
#include  "Library/Threading/Lock.hpp"
#include  "Core/Worker/Task.hh"

namespace fwk {
  namespace Core {
    namespace Worker {
      /**
       *  \class Thread Core/Worker/Thread.hh
       *  \brief A thread handling tasks.
       */
      class Thread :public Threading::Lockable, public AEndable {
      private:
        typedef void (*WorkerHandler)(Core::ATask*, bool);
        static const std::map<Core::ATask::Source, WorkerHandler> TaskHandlerMap; /*!< a map redirecting tasks to their handling methods. */

      public:
        /**
         *  \brief Cleans every tasks and clears the tasks queue.
         */
        static void cleanup();

      public:
        /**
         *  \class Assignment Core/Worker/Thread.hh
         *  \brief Enumerations of possible assignments for a worker thread.
         */
        enum class Assignment {
          TASKS, /*!< The worker thread will execute tasks from the tasks queue */
          DELAYED_TASKS /*!< This thread will put delayed tasks in the tasks queue after their delay is over */
        };

      private:
        size_t _id; /*!< ID of the thread. */
        std::thread *_thread; /*!< thread object. */

      public:
        /**
         *  \brief Constructor of Thread.
         *  \param id thread id.
         *  \param assignment assignment of the Thread.
         */
        Thread(size_t id, Assignment assignment = Assignment::TASKS);

        /**
         *  \brief Destructor of Thread.
         */
        virtual ~Thread(void);

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
        void  tasksRoutine(void);

        /**
         *  \brief the routine for workers with DELAYED_TASKS assignment.
         */
        void  delayedTasksRoutine(void);

      public:
        /**
         *  \brief Executes a SimpleTask task.
         *  \throw std::exception whatever exception the SimpleTask throws.
         *  \param task pointer to the SimpleTask.
         *  \param exec true if the task must be executed, or only cleaned if necessary.
         */
        static void  executeSimpleTask(Core::ATask*, bool exec = true);

        /**
         *  \brief Executes a EventTask task.
         *  \throw std::exception whatever exception the EventTask throws.
         *  \param task pointer to the EventTask.
         *  \param exec true if the task must be executed, or only cleaned if necessary.
         */
        static void  executeEventTask(Core::ATask*, bool exec = true);

        /**
         *  \brief Executes a HTTPTask task.
         *  \throw std::exception whatever exception the HTTPTask throws.
         *  \param task pointer to the HTTPTask.
         *  \param exec true if the task must be executed, or only cleaned if necessary.
         */
        static void  executeHTTPTask(Core::ATask*, bool exec = true);

        /**
         *  \brief Executes a PeriodicTask task.
         *  \throw std::exception whatever exception the PeriodicTask throws.
         *  \param task pointer to the PeriodicTask.
         *  \param exec true if the task must be executed, or only cleaned if necessary.
         */
        static void  executePeriodicTask(Core::ATask*, bool exec = true);
      };
    }
  }
}

#endif    /* __CORE_WORKER_WORKER_HH__ */
