#ifndef   __CORE_EVENT_EVENTINFO_HH__
#define   __CORE_EVENT_EVENTINFO_HH__

#include  <functional>
#include  <map>

#include  "Core/Event/IEventArgs.hh"
#include  "Core/Event/Event.hh"

namespace   Core {
  namespace Event {
    struct  EventInfo {
    public:
      const Core::Event::Event* base;
      std::function<void (Core::Event::IEventArgs*)> cleanup;
      std::map<const void *, std::function<void (const Core::Event::IEventArgs*)>> subscribers;

    public:
      EventInfo(const Core::Event::Event*);
      EventInfo(const Core::Event::Event*, const std::function<void (Core::Event::IEventArgs*)>&);
      EventInfo(const EventInfo&);
      EventInfo& operator=(const EventInfo&);

    public:
      void addSubscriber(const void *, const std::function<void (const Core::Event::IEventArgs*)>&);
      void delSubscriber(const void *);
    };
  }
}

#endif    /* __CORE_EVENT_EVENTINFO_HH__ */
