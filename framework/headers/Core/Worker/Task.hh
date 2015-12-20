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
    class  ATask :public Factory::AFactored {
    public:
      enum class Source {
        SIMPLE,
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

    class  SimpleTask :public ATask, public Factory::TPooled<Core::Worker::SimpleTask, 15, 10> {
    public:
      std::function<void (void)> _callback;
      std::function<void (void)> _cleanup;

    public:
      SimpleTask(void);
      virtual ~SimpleTask(void);

    public:
      virtual void reinit(void);

    public:
      void init(const std::function<void (void)>& callback);
      void init(const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
    };

    class  EventTask :public ATask, public Factory::TPooled<Core::Worker::EventTask, 100, 20> {
    public:
      std::chrono::steady_clock::time_point _eventCreation;
      const Core::Event::Event* _event;
      Core::Event::IEventArgs*  _args;

    public:
      EventTask(void);
      virtual ~EventTask(void);

    public:
      virtual void reinit(void);

    public:
      void init(const Core::Event::Event*, Core::Event::IEventArgs*);
    };

    class  HTTPTask :public ATask, public Factory::TPooled<Core::Worker::HTTPTask, 20, 5> {
    public:
      std::function<void (const Core::Network::HTTP::Response*)> _callback;
      std::function<void (void)> _cleanup;
      Core::Network::HTTP::Response* _response;

    public:
      HTTPTask(void);
      virtual ~HTTPTask(void);

    public:
      virtual void reinit(void);

    public:
      void init(const std::function<void (const Core::Network::HTTP::Response*)>&, const std::function<void (void)>&, Core::Network::HTTP::Response*); // same
    };

    class  PeriodicTask :public ATask, public Factory::TPooled<Core::Worker::PeriodicTask, 10, 2> {
    public:
      std::function<void (void)> _callback;
      std::function<void (void)> _clean;
      std::chrono::steady_clock::duration _interval;
      bool  _off;

    public:
      PeriodicTask(void);
      virtual ~PeriodicTask(void);

    public:
      virtual void reinit(void);

    public:
      void  init(const std::function<void(void)>&, const std::function<void(void)>&, const std::chrono::steady_clock::duration&);
      void  stop(bool);
    };

    class  DelayedTask :public Factory::AFactored, public Factory::TPooled<Core::Worker::DelayedTask, 50, 10> {
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
    };
  }
}

#endif    /* __CORE_WORKER_TASK_HH__ */
