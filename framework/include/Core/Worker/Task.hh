#ifndef    __CORE_WORKER_TASK_HH__
#define    __CORE_WORKER_TASK_HH__

#include  <list>
#include  <chrono>
#include  <functional>

#include  "Library/Collection/OrderedList.hpp"
#include  "Library/Factory/APooled.hpp"
#include  "Library/Threading/Notifiable.hpp"
#include  "Core/Network/Http/HttpResponse.hh"

namespace fwk {
    /**
     *  \class ATask Core/Worker/Task.hh
     *  \brief Superclass of all tasks of the tasks queue.
     */
    class ATask {
    public:
        /**
         *  \enum Source Core/Worker/Task.hh
         *  \brief Enumerations of tasks.
         */
        enum class Source {
            SIMPLE, /*!< SimpleTask */
            PERIODIC_TASK /*!< PeriodicTask */
        };

    private:
        Source  _source; /*!< the type of task. */

    protected:
        const void* _key; /*!< the key used to purge. */

    public:
        std::list<TNotifiable<std::list<ATask*>>::const_iterator>::const_iterator _taskIterator; /*!< if the key is not null, this is the iterator to the task in the task queue. */

    public:
        /**
         *  \brief Constructor of ATask.
         *  \param source the task type.
         */
        ATask(Source source);

        /**
         *  \brief Destructor of ATask.
         */
        virtual ~ATask(void);

    public:
        /**
         *  \brief Reinits the task by setting the key and the iterator to null values.
         */
        void reinit(void);

    public:
        /**
         *  \return the type of task.
         */
        Source  getSource(void) const;

        /**
         *  \return the key used to purge the task queue.
         */
        const void* getKey(void) const;
    };

    /**
     *  \class SimpleTask Core/Worker/Task.hh
     *  \brief A task with a simple callback to be executed by a worker thread.
     */
    class SimpleTask :public ATask, public APooled<SimpleTask> {
    public:
        std::function<void (void)> _callback; /*!< the callback to be called by the worker thread. */
        std::function<void (void)> _cleanup; /*!< the callback used to clean resources. Called when the tasks queue is being cleared. */

    public:
        /**
         *  \brief Constructor of SimpleTask.
         */
        SimpleTask(void);

        /**
         *  \brief Destructor of SimpleTask.
         */
        virtual ~SimpleTask(void);

    public:
        /**
         *  \brief Sets the callbacks to nullptr.
         */
        virtual void  reinit(void);

    public:
        /**
         *  \brief Sets the callback. Sets the cleanup method to nullptr.
         *  \param key the key used to purge the task queue.
         *  \param callback the callback to be executed by a worker thread.
         */
        void  init(const void* key, const std::function<void (void)>& callback);

        /**
         *  \brief Sets the callback and cleanup method.
         *  \param key the key used to purge the task queue.
         *  \param callback the callback to be executed by a worker thread.
         *  \param cleanup the callback to be used to clean resources. Called when the tasks queue is being cleared.
         */
        void  init(const void* key, const std::function<void (void)>& callback, const std::function<void (void)>& cleanup);
    };

    /**
     *  \class PeriodicTask Core/Worker/Task.hh
     *  \brief A task to be executing at regular interval.
     */
    class PeriodicTask :public ATask, public APooled<PeriodicTask> {
    public:
        std::function<void (void)> _callback; /*!< the function to call at regular interval. */
        std::function<void (void)> _cleanup; /*!< the function to call when the task is canceled or when the tasks queue is being cleared. */
        std::chrono::steady_clock::duration _interval; /*!< the duration between 2 executions of the task. */
        bool  _off; /*!< when set to true, the next execution of the task will call the clean function instead, and the task will be removed. */

    public:
        /**
         *  \brief Constructor of PeriodicTask.
         */
        PeriodicTask(void);

        /**
         *  \brief Destructor of PeriodicTask.
         */
        virtual ~PeriodicTask(void);

    public:
        /**
         *  \brief Sets all variables to their null values.
         */
        virtual void  reinit(void);

    public:
        /**
         *  \brief Sets the callback function, cleanup function and interval duration.
         *  \param key the key used to purge the task queue.
         *  \param callback the function to call at regular interval.
         *  \param cleanup the function to call when the periodic task is canceled.
         *  \param interval the interval between 2 executions of the task.
         */
        void  init(const void* key, const std::function<void(void)>& callback, const std::function<void(void)>& cleanup, const std::chrono::steady_clock::duration& interval);

        /**
         *  \brief The next execution of the task will call the cleanup function instead, and the task will be removed.
         */
        void  stop();
    };

    /**
     *  \class DelayedTask Core/Worker/Task.hh
     *  \brief A task to be executed after a delay.
     */
    class DelayedTask :public APooled<DelayedTask> {
    public:
        ATask*  _task; /*!< the task to be executed after the delay. */
        std::chrono::steady_clock::time_point  _timePoint; /*!< the timepoint when the task must be added to the tasks queue. */
        std::list<TNotifiable<OrderedList<DelayedTask*>>::const_iterator>::const_iterator _delayedTaskIterator; /*!< the iterator to the delayed task in the map. */

    public:
        /**
         *  \brief Constructor of DelayedTask.
         */
        DelayedTask(void);

        /**
         *  \brief Destructor of DelayedTask.
         */
        virtual ~DelayedTask(void);

    public:
        /**
         *  \brief Sets the task to nullptr.
         */
        virtual void  reinit(void);

    public:
        /**
         *  \brief Sets the task and time point.
         *  \param task the task to be added to the task queue at the time point.
         *  \param tp the time point when the task must be added to the task queue.
         */
        void  init(ATask* task, const std::chrono::steady_clock::time_point& tp);

        /**
         *  \brief Sets the task and time point.
         *  \param task the task to be added to the task queue after the duration.
         *  \param duration the duration to wait before adding the task to the task queue.
         */
        void  init(ATask* task, const std::chrono::steady_clock::duration& duration);

    public:
        /**
         *  \param oth the DelayedTask to compare.
         *  \return true if the timepoint of the given delayed tasks is higher than the timepoint of the current object.
         */
        bool  operator<(const DelayedTask& oth) const;

        /**
         *  \param oth the DelayedTask to compare.
         *  \return true if the timepoint of the given delayed tasks is lower than the timepoint of the current object.
         */
        bool  operator>(const DelayedTask& oth) const;

        /**
         *  \param task the task to compare.
         *  \return true if the given task is the task to add to the task queue of the current object.
         */
        bool  operator==(const ATask* task) const;
    };
}

#endif    /* __CORE_WORKER_TASK_HH__ */
