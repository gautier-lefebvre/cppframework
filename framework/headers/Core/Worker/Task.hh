#ifndef    __CORE_WORKER_TASK_HH__
#define    __CORE_WORKER_TASK_HH__

#include  <chrono>

#include  "Core/Event/Event.hh"
#include  "Core/Event/IEventArgs.hh"

namespace    Core {
  namespace  Worker {
    class  ATask :public Factory::AFactored {
    public:
      enum class Source {
        EVENT,
        HTTP_CALLBACK,
        PERIODIC_TASK
      };

    private:
      Source  _source;

    public:
      ATask(Source);
      virtual ~ATask(void);

    public:
      virtual void reinit(void) = 0;

    public:
      Source  getSource(void) const;
    };

    class  EventTask :public ATask {
    public:
      std::chrono::steady_clock::time_point  _eventCreation;
      const Core::Event::Event*        _event;
      Core::Event::IEventArgs*        _args;

    public:
      EventTask(void);
      virtual ~EventTask(void);

    public:
      virtual void reinit(void);

    public:
      void init(const Core::Event::Event*, Core::Event::IEventArgs*);

    public:
      static Factory::Pool<Core::Worker::EventTask> *Pool;
    };

    class  HTTPTask :public ATask {
    public:
      // set http args

    public:
      EventTask(void);
      virtual ~EventTask(void);

    public:
      virtual void reinit(void);

    public:
      void init(void); // same

    public:
      static Factory::Pool<Core::Worker::HTTPTask> *Pool;
    };

    class  PeriodicTask :public ATask {
    public:
      std::function<void (void)>  _callback;
      std::function<void (void)>  _clean;
      std::chrono::steady_clock::duration  _interval;
      bool  _off;

    public:
      PeriodicTask(void);
      virtual ~PeriodicTask(void);

    public:
      virtual void reinit(void);

    public:
      void  init(const std::function<void(void)>&, const std::function<void(void)>&, const std::chrono::steady_clock::duration&);
      void  stop(bool);

    public:
      static Factory::Pool<Core::Worker::PeriodicTask> *Pool;
    };

    class  DelayedTask :public Factory::AFactored {
    public:
      ATask*  _task;
      std::chrono::steady_clock::time_point  _timePoint;

    public:
      DelayedTask(void);
      virtual ~DelayedTask(void);

    public:
      virtual void reinit(void);

    public:
      void  init(ATask*, const std::chrono::steady_clock::time_point&);
      void  init(ATask*, const std::chrono::steady_clock::duration&);

    public:
      bool operator<(const DelayedTask&) const;
      bool operator>(const DelayedTask&) const;
      bool operator==(const ATask*) const;

    public:
      static Factory::Pool<Core::Worker::DelayedTask> *Pool;
    };
  }
}

#endif    /* __CORE_WORKER_TASK_HH__ */
