#ifndef   __CORE_EVENT_EVENT_HH__
#define   __CORE_EVENT_EVENT_HH__

#include  "Library/Factory/AFactored.hh"
#include  "Library/Factory/Pool.hpp"

namespace   Core {
  namespace Event {
    enum class Source {
      UNSET,
      CORE,
      APP
    };

    class Event :public Factory::AFactored, public Factory::HasBasicPool<Core::Event::Event, 50, 10> {
    private:
      Source _source;

    public:
      Event(void);
      virtual ~Event(void);

    public:
      virtual void reinit(void);

    public:
      void init(Source);

    public:
      Source getSource(void) const;
    };

    struct HasEvent {
    public:
      Core::Event::Event *event;
    };
  }
}

#endif    /* __CORE_EVENT_EVENT_HH__ */
