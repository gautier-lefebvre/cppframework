#ifndef    __CORE_WORKER_TASK_HH__
#define    __CORE_WORKER_TASK_HH__

#include  <chrono>
#include  <functional>

#include  "Library/Factory/APooled.hpp"
#include  "Core/Event/EventHandle.hh"
#include  "Core/Event/IEventArgs.hh"
#include  "Core/Network/Http/HttpResponse.hh"

namespace fwk {
  /**
   *  \class ATask Core/Worker/Task.hh
   *  \brief Superclass of all tasks of the tasks queue.
   */
  class ATask {
  public:
    /**
     *  \class Source Core/Worker/Task.hh
     *  \brief Enumerations of tasks.
     */
    enum class Source {
      SIMPLE, /*!< SimpleTask */
      EVENT, /*!< EventTask */
      HTTP_CALLBACK, /*!< HttpTask */
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
     *  \return the type of task.
     */
    Source  getSource(void) const;
  };

  /**
   *  \class SimpleTask Core/Worker/Task.hh
   *  \brief A task with a simple callback to be executed by a worker thread.
   */
  class SimpleTask :public ATask, public APooled<SimpleTask> {
  public:
    std::function<void (void)> _callback; /*!< the callback to be called by the worker thread. */
    std::function<void (void)> _cleanup; /*!< the callback used to clean resources. Called when the tasks queue is being cleared. */

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
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the callback. Sets the cleanup method to nullptr.
     *  \param callback the callback to be executed by a worker thread.
     */
    void  init(const std::function<void (void)>& callback);

    /**
     *  \brief Sets the callback and cleanup method.
     *  \param callback the callback to be executed by a worker thread.
     *  \param cleanup the callback to be used to clean resources. Called when the tasks queue is being cleared.
     */
    void  init(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
  };

  /**
   *  \class EventTask Core/Worker/Thread.hh
   *  \brief A task created when an event is fired.
   */
  class EventTask :public ATask, public APooled<EventTask> {
  public:
    std::chrono::steady_clock::time_point _eventCreation; /*!< time of event firing. */
    const EventHandle* _event; /*!< the event fired. */
    IEventArgs*        _args; /*!< the event arguments. */

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
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the event and its arguments.
     *  \param event the fired event.
     *  \param args the event arguments.
     */
    void  init(const EventHandle* event, IEventArgs* args);
  };

  /**
   *  \class HttpTask Core/Worker/Task.hh
   *  \brief A task created after a HTTP response has been received.
   */
  class HttpTask :public ATask, public APooled<HttpTask> {
  public:
    std::function<void (const HttpResponse*)> _callback; /*!< the callback function to call with the response. */
    std::function<void (void)> _cleanup; /*!< the method used to clean resources used with the request. */
    HttpResponse* _response; /*!< the response received. */

  public:
    /**
     *  \brief Constructor of HttpTask.
     */
    HttpTask(void);

    /**
     *  \brief Destructor of HttpTask.
     */
    virtual ~HttpTask(void);

  public:
    /**
     *  \brief Sets the attributes to their null values.
     */
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the callback function, the cleanup function and the HTTP response.
     *  \param callback the callback method to call.
     *  \param cleanup the cleanup method used to clean resources used in the request. It is only called when the tasks queue is being cleared, so never after the callback method.
     *  \param response the HTTP response received.
     */
    void  init(const std::function<void (const HttpResponse*)>& callback, const std::function<void (void)>& cleanup, HttpResponse* response);
  };

  /**
   *  \class PeriodicTask Core/Worker/Task.hh
   *  \brief A task to be executing at regular interval.
   */
  class PeriodicTask :public ATask, public APooled<PeriodicTask> {
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
    virtual void  reinit(void);

  public:
    /**
     *  \brief Sets the callback function, cleanup function and interval duration.
     *  \param callback the function to call at regular interval.
     *  \param clean the function to call when the periodic task is canceled.
     *  \param interval the interval between 2 executions of the task.
     */
    void  init(const std::function<void(void)>& callback, const std::function<void(void)>& clean, const std::chrono::steady_clock::duration& interval);

    /**
     *  \brief The next execution of the task will call the clean function instead, and the task will be removed.
     */
    void  stop();
  };

  /**
   *  \class DelayedTask Core/Worker/Thread.hh
   *  \brief A task to be executed after a delay.
   */
  class DelayedTask :public APooled<DelayedTask> {
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
    virtual void  reinit(void);

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
    bool  operator<(const DelayedTask& oth) const;

    /**
     *  \param oth the DelayedTask to compare.
     *  \return true if the timepoint of the given delayed tasks is lower than the timepoint of the current object.
     */
    bool  operator>(const DelayedTask& oth) const;

    /**
     *  \param task the task to compare.
     *  \return true if the given task is the task to add to the task queue of the current object.
     */
    bool  operator==(const ATask* task) const;
  };
}

#endif    /* __CORE_WORKER_TASK_HH__ */
