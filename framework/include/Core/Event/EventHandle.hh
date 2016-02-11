#ifndef   __CORE_EVENT_EVENTHANDLE_HH__
#define   __CORE_EVENT_EVENTHANDLE_HH__

#include  "Library/Factory/APooled.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Core/Event/IEventArgs.hh"

namespace fwk {
  /**
   *  \class EventHandle Core/Event/EventHandle.hh
   *  \brief an Event of the program.
   *  Pooled.
   */
  class EventHandle :public APooled<EventHandle> {
  public:
    /**
     *  \brief Constructor of EventHandle.
     */
    EventHandle(void);

  public:
    /**
     *  \brief Inherited from APooled. Does nothing.
     */
    virtual void reinit(void);

  public:
    /**
     *  \brief Shortcut to EventManager::get().fireAsync(event, args);
     *  See EventManager::fireAsync to see the behaviour of the method and the exceptions thrown.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireAsync(IEventArgs* args) const;

    /**
     *  \brief Shortcut to EventManager::get().fireAsync(event, args);
     *  See EventManager::fireSync to see the behaviour of the method and the exceptions thrown.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireSync(IEventArgs* args) const;

    /**
     *  \brief Shortcut to EventManager::get().subscribeToEvent(event, callback, key).
     *  \param callback the function called when the event is fired.
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  subscribe(const std::function<void (const IEventArgs*)>& callback, const void *key) const;

    /**
     *  \brief Shortcut to EventManager::get().unsubscribeFromEvent(event, key).
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  unsubscribe(const void *key) const;

    /**
     *  \brief Shortcut to EventManager::get().registerEvent(event).
     */
    void  registerToManager(void) const;

    /**
     *  \brief Shortcut to EventManager::get().unregisterEvent(event).
     */
    void  unregisterFromManager(void) const;
  };
}

#endif    /* __CORE_EVENT_EVENTHANDLE_HH__ */
