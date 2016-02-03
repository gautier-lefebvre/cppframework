#ifndef   __CORE_EVENT_HANDLE_HH__
#define   __CORE_EVENT_HANDLE_HH__

#include  "Library/Factory/AFactored.hh"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Event/IEventArgs.hh"

namespace   Core {
  namespace Event {
    /**
     *  \class Handle Core/Event/Handle.hh
     *  \brief an Event of the program.
     *  Pooled.
     */
    class Handle :public Factory::AFactored, public Factory::TPooled<Core::Event::Handle, 50, 10> {
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

      /**
       *  \brief Shortcut to Core::Event::Manager::get().subscribeToEvent(event, callback, key).
       *  \param callback the function called when the event is fired.
       *  \param key the key of the subscriber (must be unique for this event).
       */
      void subscribe(const std::function<void (const Core::Event::IEventArgs *)>& callback, const void *key) const;

      /**
       *  \brief Shortcut to Core::Event::Manager::get().unsubscribeFromEvent(event, key).
       *  \param key the key of the subscriber (must be unique for this event).
       */
      void unsubscribe(const void *key) const;

      /**
       *  \brief Shortcut to Core::Event::Manager::get().registerEvent(event).
       */
      void registerToManager(void) const;

      /**
       *  \brief Shortcut to Core::Event::Manager::get().unregisterEvent(event).
       */
      void unregisterFromManager(void) const;
    };
  }
}

#endif    /* __CORE_EVENT_HANDLE_HH__ */
