#ifndef   __CORE_EVENT_EVENTINFO_HH__
#define   __CORE_EVENT_EVENTINFO_HH__

#include  <functional>
#include  <map>

#include  "Core/Event/IEventArgs.hh"
#include  "Core/Event/EventHandle.hh"

namespace fwk {
  /**
   *  \class EventInfo Core/Event/EventInfo.hh
   *  \brief Structure containing information about a registered Event.
   */
  struct EventInfo {
  public:
    const EventHandle * base; /*!< the registered event. */
    std::map<const void *, std::function<void (const IEventArgs*)>> subscribers; /*!< the subscribers of the event. */

  public:
    /**
     *  \brief Constructor of EventInfo.
     *  \param event the event.
     */
    EventInfo(const EventHandle *event);

    /**
     *  \brief Copy constructor of EventInfo.
     *  Copies the pointer to the event and the subscribers.
     *  \param oth the EventInfo object to copy.
     */
    EventInfo(const EventInfo& oth);

    /**
     *  \brief Assignment constructor of EventInfo.
     *  Copies the pointer to the event and the subscribers.
     *  \param oth the EventInfo object to copy.
     */
    EventInfo& operator=(const EventInfo& oth);

  public:
    /**
     *  \brief Adds a subscriber to the event.
     *  \param key the key of the subscriber.
     *  \param callback the callback method to call when the event is fired.
     */
    void  addSubscriber(const void* key, const std::function<void (const IEventArgs*)>& callback);

    /**
     *  \brief Removes a subscriber from the event.
     *  \param key the key used when subscribing.
     */
    void  delSubscriber(const void* key);
  };
}

#endif    /* __CORE_EVENT_EVENTINFO_HH__ */
