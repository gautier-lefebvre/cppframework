#ifndef   __CORE_EVENT_EVENT_HH__
#define   __CORE_EVENT_EVENT_HH__

#include  "Library/Factory/AFactored.hh"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Event/IEventArgs.hh"

namespace   Core {
  namespace Event {
    /**
     *  \class Event Core/Event/Event.hh
     *  \brief an Event of the program.
     *  Pooled.
     */
    class Event :public Factory::AFactored, public Factory::TPooled<Core::Event::Event, 50, 10> {
    public:
      /**
       *  \brief Inherited from AFactored. Does nothing.
       */
      virtual void reinit(void);

    public:
      /**
       *  \brief Shortcut to Core::Event::Manager::get().fireAsync(event, args);
       *  See Core::Event::Manager::fireAsync to see the behaviour of the method and the exceptions thrown.
       *  \param args the arguments passed to the subscribers of the event.
       */
      void fireAsync(Core::Event::IEventArgs *args) const;

      /**
       *  \brief Shortcut to Core::Event::Manager::get().fireAsync(event, args);
       *  See Core::Event::Manager::fireSync to see the behaviour of the method and the exceptions thrown.
       *  \param args the arguments passed to the subscribers of the event.
       */
      void fireSync(Core::Event::IEventArgs *args) const;
    };
  }
}

#endif    /* __CORE_EVENT_EVENT_HH__ */
