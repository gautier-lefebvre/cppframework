#ifndef   __CORE_EVENT_EVENTHANDLE_HH__
#define   __CORE_EVENT_EVENTHANDLE_HH__

#include  "Library/Factory/APooled.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Library/Threading/Lockable.hpp"
#include  "Core/Event/IEventArgs.hh"

namespace fwk {
  /**
   *  \class EventHandle Core/Event/EventHandle.hh
   *  \brief an Event of the program.
   *  Pooled.
   */
  class EventHandle :public APooled<EventHandle>, public Lockable {
  private:
    std::map<const void *, std::function<void (const IEventArgs*)>> _subscribers; /*!< the subscribers of the event. */

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
     *  \brief Adds an EventTask to the task queue. The event will be executed asynchronously.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireAsync(IEventArgs* args);

    /**
     *  \brief Calls every subscribers.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireSync(IEventArgs* args);

    /**
     *  \brief Adds a subscriber to the event. The given key will be used to unsubscribe.
     *  \param callback the function called when the event is fired.
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  subscribe(const std::function<void (const IEventArgs*)>& callback, const void *key);

    /**
     *  \brief Removes a subscriber from the event. The given key was given when subscribing.
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  unsubscribe(const void *key);

  public:
    /**
     *  \brief Calls every callbacks. Must only be called by the worker threads.
     *  If you cant to fire an event, use fireSync or fireAsync.
     *  \param args the arguments to pass to the callbacks.
     */
    void  exec(const IEventArgs* args);
  };
}

#endif    /* __CORE_EVENT_EVENTHANDLE_HH__ */
