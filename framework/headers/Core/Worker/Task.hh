#ifndef    __CORE_WORKER_TASK_HH__
#define    __CORE_WORKER_TASK_HH__

#include  <chrono>
#include  <functional>

#include  "Library/Factory/Pool.hpp"
#include  "Core/Event/Event.hh"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Network/HTTP/Response.hh"

namespace    Core {
  namespace  Worker {
    /**
     *  \class ATask Core/Worker/Task.hh
     *  \brief Superclass of all tasks of the tasks queue.
     */
    class  ATask :public Factory::AFactored {
    public:
      /**
       *  \class Source Core/Worker/Task.hh
       *  \brief Enumerations of tasks.
       */
      enum class Source {
        SIMPLE, /*!< SimpleTask */
        EVENT, /*!< EventTask */
        HTTP_CALLBACK, /*!< HTTPTask */
        PERIODIC_TASK /*!< PeriodicTask */
      };

    private:
      Source  _source; /*!< the type of task. */

    public:
      /**
       *  \brief Constructor of ATask.
       *  \param source the task type.
       */
      ATask(Source source);

      /**
       *  \brief Destructor of ATask.
       */
      virtual ~ATask(void);

    public:
      /**
       *  \brief Inherited from Factory::AFactored. Must be implemented by subclasses.
       */
      virtual void reinit(void) = 0;

    public:
      /**
       *  \return the type of task.
       */
      Source  getSource(void) const;
    };

    /**
     *  \class SimpleTask Core/Worker/Task.hh
     *  \brief A task with a simple callback to be executed by a worker thread.
     */
    class  SimpleTask :public ATask, public Factory::TPooled<Core::Worker::SimpleTask, 15, 10> {
    public:
      std::function<void (void)> _callback; /*!< the callback to be called by the worker thread. */
      std::function<void (void)> _cleanup; /*!< the callback used to clean resources. Called after the execution callback, or when the tasks queue is being cleared. */

    public:
      /**
       *  \brief Constructor of SimpleTask.
       */
      SimpleTask(void);

      /**
       *  \brief Destructor of SimpleTask.
       */
      virtual ~SimpleTask(void);

    public:
      /**
       *  \brief Sets the callbacks to nullptr.
       */
      virtual void reinit(void);

    public:
      /**
       *  \brief Sets the callback. Sets the cleanup method to nullptr.
       *  \param callback the callback to be executed by a worker thread.
       */
      void init(const std::function<void (void)>& callback);

      /**
       *  \brief Sets the callback and cleanup method.
       *  \param callback the callback to be executed by a worker thread.
       *  \param cleanup the callback to be used to clean resources. Called after the execution callback, or when the tasks queue is being cleared.
       */
      void init(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
    };

    /**
     *  \class EventTask Core/Worker/Thread.hh
     *  \brief A task created when an event is fired.
     */
    class  EventTask :public ATask, public Factory::TPooled<Core::Worker::EventTask, 100, 20> {
    public:
      std::chrono::steady_clock::time_point _eventCreation; /*!< time of event firing. */
      const Core::Event::Event* _event; /*!< the event fired. */
      Core::Event::IEventArgs*  _args; /*!< the event arguments. */

    public:
      /**
       *  \brief Constructor of EventTask.
       */
      EventTask(void);

      /**
       *  \brief Destructor of EventTask.
       */
      virtual ~EventTask(void);

    public:
      /**
       *  brief Sets attributes to null values.
       */
      virtual void reinit(void);

    public:
      /**
       *  \brief Sets the event and its arguments.
       *  \param event the fired event.
       *  \param args the event arguments.
       */
      void init(const Core::Event::Event* event, Core::Event::IEventArgs* args);
    };

    /**
     *  \class HTTPTask Core/Worker/Task.hh
     *  \brief A task created after a HTTP response has been received.
     */
    class  HTTPTask :public ATask, public Factory::TPooled<Core::Worker::HTTPTask, 20, 5> {
    public:
      std::function<void (const Core::Network::HTTP::Response*)> _callback; /*!< the callback function to call with the response. */
      std::function<void (void)> _cleanup; /*!< the method used to clean resources used with the request. */
      Core::Network::HTTP::Response* _response; /*!< the response received. */

    public:
      /**
       *  \brief Constructor of HTTPTask.
       */
      HTTPTask(void);

      /**
       *  \brief Destructor of HTTPTask.
       */
      virtual ~HTTPTask(void);

    public:
      /**
       *  \brief Sets the attributes to their null values.
       */
      virtual void reinit(void);

    public:
      /**
       *  \brief Sets the callback function, the cleanup function and the HTTP response.
       *  \param callback the callback method to call.
       *  \param cleanup the cleanup method used to clean resources used in the request. It is only called when the tasks queue is being cleared, so never after the callback method.
       *  \param response the HTTP response received.
       */
      void init(const std::function<void (const Core::Network::HTTP::Response*)>& callback, const std::function<void (void)>& cleanup, Core::Network::HTTP::Response* response);
    };

    /**
     *  \class PeriodicTask Core/Worker/Task.hh
     *  \brief A task to be executing at regular interval.
     */
    class  PeriodicTask :public ATask, public Factory::TPooled<Core::Worker::PeriodicTask, 10, 2> {
    public:
      std::function<void (void)> _callback; /*!< the function to call at regular interval. */
      std::function<void (void)> _clean; /*!< the function to call when the task is canceled or when the tasks queue is being cleared. */
      std::chrono::steady_clock::duration _interval; /*!< the duration between 2 executions of the task. */
      bool  _off; /*!< when set to true, the next execution of the task will call the clean function instead, and the task will be removed. */

    public:
      /**
       *  \brief Constructor of PeriodicTask.
       */
      PeriodicTask(void);

      /**
       *  \brief Destructor of PeriodicTask.
       */
      virtual ~PeriodicTask(void);

    public:
      /**
       *  \brief Sets all variables to their null values.
       */
      virtual void reinit(void);

    public:
      /**
       *  \brief Sets the callback function, cleanup function and interval duration.
       *  \param callback the function to call at regular interval.
       *  \param cleanup the function to call when the periodic task is canceled.
       *  \param duration the interval between 2 executions of the task.
       */
      void  init(const std::function<void(void)>&, const std::function<void(void)>&, const std::chrono::steady_clock::duration&);

      /**
       *  \brief The next execution of the task will call the clean function instead, and the task will be removed.
       */
      void  stop();
    };

    /**
     *  \class DelayedTask Core/Worker/Thread.hh
     *  \brief A task to be executed after a delay.
     */
    class  DelayedTask :public Factory::AFactored, public Factory::TPooled<Core::Worker::DelayedTask, 50, 10> {
    public:
      ATask*  _task; /*!< the task to be executed after the delay. */
      std::chrono::steady_clock::time_point  _timePoint; /*!< the timepoint when the task must be added to the tasks queue. */

    public:
      /**
       *  \brief Constructor of DelayedTask.
       */
      DelayedTask(void);

      /**
       *  \brief Destructor of DelayedTask.
       */
      virtual ~DelayedTask(void);

    public:
      /**
       *  \brief Sets the task to nullptr.
       */
      virtual void reinit(void);

    public:
      /**
       *  \brief Sets the task and time point.
       *  \param task the task to be added to the task queue at the time point.
       *  \param tp the time point when the task must be added to the task queue.
       */
      void  init(ATask* task, const std::chrono::steady_clock::time_point& tp);

      /**
       *  \brief Sets the task and time point.
       *  \param task the task to be added to the task queue after the duration.
       *  \param duration the duration to wait before adding the task to the task queue.
       */
      void  init(ATask* task, const std::chrono::steady_clock::duration& duration);

    public:
      /**
       *  \param oth the DelayedTask to compare.
       *  \return true if the timepoint of the given delayed tasks is higher than the timepoint of the current object.
       */
      bool operator<(const DelayedTask& oth) const;

      /**
       *  \param oth the DelayedTask to compare.
       *  \return true if the timepoint of the given delayed tasks is lower than the timepoint of the current object.
       */
      bool operator>(const DelayedTask& oth) const;

      /**
       *  \param task the task to compare.
       *  \return true if the given task is the task to add to the task queue of the current object.
       */
      bool operator==(const ATask* task) const;
    };
  }
}

#endif    /* __CORE_WORKER_TASK_HH__ */
