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
    class EventHandle :public Lockable {
    private:
        std::map<const void *, std::function<void (Args...)>> _subscribers; /*!< the subscribers of the event. */

    public:
        /**
         *  \brief Constructor of EventHandle.
         */
        EventHandle(void):
            Lockable(),
            _subscribers()
        {}

        /**
         *  \brief Destructor of EventHandle. Will purge the WorkerManager of all current asynchronous triggering of this event.
         */
        ~EventHandle(void) {
            this->purgeTaskQueue();
        }

    private:
        /**
         *  \brief Creates the callback by wrapping the arguments with a lamba and capturing them.
         *  \param args the arguments passed to the subscribers of the event when it is fired.
         *  \returns the callback to give to a Task
         */
        std::function<void (void)> wrapArguments(Args&&... args) {
            return [this, &args...] (void) -> void {
                this->exec(std::forward<Args>(args)...);
            };
        }

    public:
        /**
         *  \brief Adds a SimpleTask to the task queue. The event will be executed asynchronously.
         *  \param args the arguments passed to the subscribers of the event.
         */
        void  fireAsync(Args... args) {
            SCOPELOCK(this);
        	if (this->_subscribers.empty()) { return ; }
            WorkerManager::get().addSimpleTask(this, this->wrapArguments(std::forward<Args>(args)...));
        }

        /**
         *  \brief Calls every subscribers.
         *  \param args the arguments passed to the subscribers of the event.
         */
        void  fireSync(Args... args)  {
        	SCOPELOCK(this);
        	if (this->_subscribers.empty()) { return ; }

            SimpleTask* simpleTask = nullptr;

            try {
                simpleTask = SimpleTask::getFromPool(this, this->wrapArguments(std::forward<Args>(args)...));
                WorkerThread::executeSimpleTask(simpleTask, true);
            } catch (const std::exception& e) {
                CRITICAL(e.what());
                SimpleTask::returnToPool(simpleTask);
            }
        }

        /**
         *  \brief Adds a subscriber to the event. The given key will be used to unsubscribe.
         *  \param callback the function called when the event is fired.
         *  \param key the key of the subscriber (must be unique for this event).
         */
        void  subscribe(const std::function<void (Args&&...)>& callback, const void *key) {
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

        /**
         *  \brief Purges the task queue of this event asynchronous firing.
         */
        void purgeTaskQueue() {
            SCOPELOCK(this);
            WorkerManager::get().purgeTaskQueue(this);
        }

        /**
         *  \brief Reinits this event. Clears the list of all subscribers. Purges the task queue of all tasks for this event.
         */
        void reinit() {
            SCOPELOCK(this);
            this->_subscribers.clear();
            this->purgeTaskQueue();
        }

    public:
        /**
         *  \brief Calls every callbacks. Must only be called by the worker threads.
         *  If you cant to fire an event, use fireSync or fireAsync.
         *  \param args the arguments to pass to the callbacks.
         */
        void  exec(Args&&... args) {
            SCOPELOCK(this);

            // call every subscribers
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
    };
}

#endif    /* __CORE_EVENT_EVENTHANDLE_HPP__ */
