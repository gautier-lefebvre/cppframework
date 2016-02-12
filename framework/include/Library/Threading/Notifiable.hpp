#ifndef    __LIBRARY_THREADING_NOTIFIABLE_HPP__
#define    __LIBRARY_THREADING_NOTIFIABLE_HPP__

#include  <condition_variable>
#include  <chrono>
#include  <thread>

#include  "Library/Threading/Lockable.hpp"

namespace fwk {
  /**
   *  \class Notifiable Library/Threading/Notifiable.hpp
   *  \brief A condition variable (synchronization primitive) using a reentrant lock (or recursive mutex).
   */
  class Notifiable :public Lockable {
  private:
    std::condition_variable_any _condvar; /*!< the basic condition variable. */

  public:
    /**
     *  \brief Constructor of Notifiable.
     */
    Notifiable(void);

    /**
     *  \brief Destructor of Notifiable.
     */
    virtual ~Notifiable(void);

  public:
    /**
     *  \brief Waits for the condition variable to be notified. Will unlock the mutex, wait, and lock it again before returning.
     */
    void  wait(void);

    /**
     *  \brief Wakes one thread waiting on this condition variable.
     */
    void  notify(void);

    /**
     *  \brief Wakes all threads waiting on this condition variable.
     */
    void  notify_all(void);

    /**
     *  \brief Waits until this condition variable is notified and the predicate is true.
     *  \param pred the predicate.
     */
    template <class Predicate>
    void  wait(Predicate pred) {
      this->_condvar.wait(*this, pred);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout.
     *  \param tp the point in time when the condition variable must be woken.
     */
    template<class C, class D>
    std::cv_status  wait_until(const std::chrono::time_point<C, D>& tp) {
      return this->_condvar.wait_until(*this, tp);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout, and checking that a predicate is satisfied.
     *  \param timeout_time the point in time when the condition variable must be woken.
     *  \param pred the predicate.
     */
    template<class C, class D, class P>
    bool  wait_until(const std::chrono::time_point<C, D>& timeout_time, P pred) {
      return this->_condvar(*this, timeout_time, pred);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout.
     *  \param rl the maximum duration this condition variable is locked.
     */
    template<class R, class P>
    std::cv_status  wait_for(const std::chrono::duration<R, P>& rl) {
      return this->_condvar.wait_for(*this, rl);
    }

    /**
     *  \brief Waits until this condition variable is notified, with a timeout, and checking that a predicate is satisfied.
     *  \param rel_time the maximum duration this condition variable is locked.
     *  \param pred the predicate.
     */
    template<class R, class Per, class Pre>
    bool  wait_for(const std::chrono::duration<R, Per>& rel_time, Pre pred) {
      return this->_condvar.wait_for(*this, rel_time, pred);
    }
  };

  /**
   *  \class TNotifiable Library/Threading/Notifiable.hpp
   *  \brief A templated class to use when setting an object notifiable.
   *  Can be useful for notifying object of the STL, like `TNotifiable<std::list<int>>`.
   */
  template<class C>
  class TNotifiable :public C, public Notifiable {
  public:
    /**
     *  \brief Default constructor of TNotifiable.
     *  \param args the arguments which will be passed to the constructor of the templated class.
     */
    template<typename... Args>
    TNotifiable(const Args&... args):
      C(args...),
      Notifiable()
    {}

    /**
     *  \brief Destructor of TNotifiable.
     */
    virtual ~TNotifiable(void) {}
  };

  /**
   *  \class NotifiableThread Library/Threading/Notifiable.hpp
   *  \brief A struct containing a pointer to a thread, and a Notifiable.
   *
   *  I'm not using Notifiable<std::thread*> because I'm not sure of everything this implies.
   */
  struct NotifiableThread {
    std::thread* thread; /*!< the thread. */
    Notifiable   condition; /*!< the condition variable. */
  };
}

#endif    /* __LIBRARY_THREADING_NOTIFIABLE_HPP__ */
