#ifndef   __CORE_EVENT_EVENTHANDLE_HPP__
#define   __CORE_EVENT_EVENTHANDLE_HPP__

#include  "Library/Factory/APooled.hpp"
#include  "Library/Factory/Pool.hpp"
#include  "Library/Threading/Lockable.hpp"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/Worker/WorkerThread.hh"

namespace fwk {
  /**
   *  \class EventHandle Core/Event/EventHandle.hpp
   *  \brief an Event of the program.
   *  Pooled.
   */
  template<typename... Args>
  class EventHandle :public APooled<EventHandle<Args...>>, public Lockable {
  private:
    std::map<const void *, std::function<void (Args...)>> _subscribers; /*!< the subscribers of the event. */

  public:
    /**
     *  \brief Constructor of EventHandle.
     */
    EventHandle(void):
      APooled<EventHandle<Args...>>(),
      Lockable(),
      _subscribers()
    {}

  public:
    /**
     *  \brief Inherited from APooled. Does nothing.
     */
    virtual void reinit(void) {
      SCOPELOCK(this);
      this->_subscribers.clear();
    }

  private:
    /**
     *  \brief Creates the callback by wrapping the arguments with a lamba and capturing them.
     *  \param args the arguments passed to the subscribers of the event when it is fired.
     *  \returns the callback to give an EventTask
     */
    std::function<void (const std::chrono::steady_clock::time_point&)> wrapArguments(Args... args) {
      return [this, &args...] (const std::chrono::steady_clock::time_point& eventTaskTimePoint) -> void {
        this->exec(eventTaskTimePoint, std::forward<Args>(args)...);
      };
    }

  public:
    /**
     *  \brief Adds an EventTask to the task queue. The event will be executed asynchronously.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireAsync(Args... args) {
      SCOPELOCK(this);
      WorkerManager::get().addEventTask(this->lastOutOfPoolTimePoint(), this->wrapArguments(std::forward<Args>(args)...));
    }

    /**
     *  \brief Calls every subscribers.
     *  \param args the arguments passed to the subscribers of the event.
     */
    void  fireSync(Args... args)  {
      EventTask* eventTask = nullptr;

      try {
        eventTask = EventTask::getFromPool(this->lastOutOfPoolTimePoint(), this->wrapArguments(std::forward<Args>(args)...));
        WorkerThread::executeEventTask(eventTask, true);
      } catch (const std::exception& e) {
        CRITICAL(e.what());
        EventTask::returnToPool(eventTask);
      }
    }

    /**
     *  \brief Adds a subscriber to the event. The given key will be used to unsubscribe.
     *  \param callback the function called when the event is fired.
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  subscribe(const std::function<void (Args&&...)>& callback, const void *key){
      SCOPELOCK(this);
      this->_subscribers[key] = callback;
    }

    /**
     *  \brief Removes a subscriber from the event. The given key was given when subscribing.
     *  \param key the key of the subscriber (must be unique for this event).
     */
    void  unsubscribe(const void *key) {
      SCOPELOCK(this);
      this->_subscribers.erase(key);
    }

  public:
    /**
     *  \brief Calls every callbacks. Must only be called by the worker threads.
     *  If you cant to fire an event, use fireSync or fireAsync.
     *  \param eventTaskTimePoint the timepoint when the EventTask was created. If this timepoint is different than this->lastOutOfPoolTimePoint(), then the EventTask must not be executed.
     *  \param args the arguments to pass to the callbacks.
     */
    void  exec(const std::chrono::steady_clock::time_point& eventTaskTimePoint, Args&&... args) {
      SCOPELOCK(this);

      // execute only if the event was not reinit between being added to the task queue and being executed.
      if (this->isValid() && eventTaskTimePoint == this->lastOutOfPoolTimePoint()) {
        for (auto& it : this->_subscribers) {
          try {
            if (it.second) {
              it.second(args...);
            }
          } catch (const std::exception& e) {
            CRITICAL(e.what());
          }
        }
      }
    }
  };
}

#endif    /* __CORE_EVENT_EVENTHANDLE_HPP__ */
