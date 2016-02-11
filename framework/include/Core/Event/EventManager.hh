#ifndef    __CORE_EVENT_EVENTMANAGER_HH__
#define    __CORE_EVENT_EVENTMANAGER_HH__

#include  <functional>
#include  <unordered_map>

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Threading/Lockable.hpp"
#include  "Core/Event/EventInfo.hh"
#include  "Core/Event/Exception.hh"

namespace fwk {
  /**
   *  \class EventManager Core/Event/EventManager.hh
   *  \brief Singleton class used to register events and subscribe to them.
   */
  class EventManager :public Singleton<fwk::EventManager>, public Lockable {
    friend class Singleton<fwk::EventManager>;
  private:
    std::unordered_map<const EventHandle*, EventInfo>  _events; /*!< registered events and their related information. */

  private:
    /**
     *  \brief Deleted copy constructor of EventManager.
     */
    EventManager(const EventManager&) = delete;

    /**
     *  \brief Deleted move constructor of EventManager.
     */
    EventManager(const EventManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of EventManager.
     */
    EventManager& operator=(const EventManager&) = delete;

  private:
    /**
     *  \brief Constructor of EventManager.
     */
    EventManager(void);

    /**
     *  \brief Destructor of EventManager.
     */
    virtual ~EventManager(void);

  public:
    /**
     *  \brief Registers an event. An event must be registered before it can be subscribed to.
     *  \param event the event to register.
     */
    void  registerEvent(const EventHandle* event);

    /**
     *  \brief Unregisters an event.
     *  \param event the event to unregister.
     */
    void  unregisterEvent(const EventHandle* event);

    /**
     *  \brief Subscribe to an event. An event must be registered before it can be subscribe to.
     *  \throw EventNotRegisteredException the event is not currently registered.
     *  \param event the event to subscribe to.
     *  \param callback the method to call when the event is fired.
     *  \param key the key of the caller, used to store the callback. The easiest solution is to send the 'this' pointer. No point in subscribing twice to the same event for the same object.
     *  The argument of the callback must be recasted (using reinterpret_cast) to the subclass of IEventArgs. See the related event to know its true type.
     */
    void  subscribeToEvent(const EventHandle* event, const std::function<void (const IEventArgs*)>& callback, const void *key);

    /**
     *  \brief Unsubscribe from an event. The event must have been registered and subscribed to.
     *  \throw EventNotRegisteredException the event is not currently registered.
     *  \param event the event to unsubscribed from.
     *  \param key the key used when subscribing to the event.
     */
    void  unsubscribeFromEvent(const EventHandle* event, const void *key);

  public:
    /**
     *  \brief Fire the event asynchronously, calling the callback method of every subscriber with the given arguments.
     *  This actually just adds a new task to the worker tasks queue. It will be executed after any other task has been cleared.
     *  \param event the event to fire.
     *  \param args the args to send to the callback method. Its real type must be the same for each call.
     */
    void  fireEventAsync(const EventHandle* event, IEventArgs* args) const;

    /**
     *  \brief Fire the event synchronously, calling the callback method of every subscriber with the given arguments.
     *  This stops the calling thread so be sure of what you are doing.
     *  \param event the event to fire.
     *  \param args the args to send to the callback method. Its real type must be the same for each call.
     *  Note that the arguments are no longer valid after this method is called. Don't use them. Any thing you put inside will be erased whenever another thread pulls it from the pool.
     */
    void  fireEventSync(const EventHandle* event, IEventArgs* args) const; // note that the args are no longer valid after this method is called

  public:
    /**
     *  \brief Returns the information of an event (its subscribers).
     *  \throw EventNotRegisteredException the event is not currently registered.
     *  \param event the event whose information to retrieve.
     *  \return the information of the event.
     */
    const EventInfo&  getInfo(const EventHandle* event) const;
  };
}

#endif    /* __CORE_EVENT_MANAGER_HH__ */
