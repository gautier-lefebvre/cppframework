#ifndef		__LIBRARY_THREADING_CONDITION_HPP__
#define		__LIBRARY_THREADING_CONDITION_HPP__

#include	<condition_variable>
#include	<chrono>

#include	"Library/Threading/Lock.hpp"

namespace	Threading {
	/**
	 *	\class Condition Library/Threading/Condition.hpp
	 *	\brief A condition variable (synchronization primitive) using a reentrant lock (or recursive mutex).
	 */
	class	Condition :public Threading::Lock {
	private:
		std::condition_variable_any _condvar; /*!< the basic condition variable. */

	public:
		/**
		 *	\brief Constructor of Condition.
		 */
		Condition();

		/**
		 *	\brief Destructor of Condition.
		 */
		virtual ~Condition();

	public:
		/**
		 *	\brief Waits for the condition variable to be notified. Will unlock the mutex, wait, and lock it again before returning.
		 */
		void	wait();

		/**
		 *	\brief Wakes one thread waiting on this condition variable.
		 */
		void	notify();

		/**
		 *	\brief Wakes all threads waiting on this condition variable.
		 */
		void	notify_all();

		/**
		 *	\brief Waits until this condition variable is notified and the predicate is true.
		 *	\param pred the predicate.
		 */
		template <class Predicate>
		void wait (Predicate pred) {
			this->_condvar.wait(*this, pred);
		}

		/**
		 *	\brief Waits until this condition variable is notified, with a timeout.
		 *	\param tp the point in time when the condition variable must be woken.
		 */
		template<class C, class D>
		std::cv_status	wait_until(const std::chrono::time_point<C, D>& tp) {
			return this->_condvar.wait_until(*this, tp);
		}

		/**
		 *	\brief Waits until this condition variable is notified, with a timeout, and checking that a predicate is satisfied.
		 *	\param timeout_time the point in time when the condition variable must be woken.
		 *	\param pred the predicate.
		 */
		template<class C, class D, class P>
		bool wait_until(const std::chrono::time_point<C, D>& timeout_time, P pred) {
			return this->_condvar(*this, timeout_time, pred);
		}

		/**
		 *	\brief Waits until this condition variable is notified, with a timeout.
		 *	\param rl the maximum duration this condition variable is locked.
		 */
		template<class R, class P>
		std::cv_status	wait_for(const std::chrono::duration<R, P>& rl) {
			return this->_condvar.wait_for(*this, rl);
		}

		/**
		 *	\brief Waits until this condition variable is notified, with a timeout, and checking that a predicate is satisfied.
		 *	\param rel_time the maximum duration this condition variable is locked.
		 *	\param pred the predicate.
		 */
		template<class R, class Per, class Pre>
		bool wait_for(const std::chrono::duration<R, Per>& rel_time, Pre pred) {
			return this->_condvar.wait_for(*this, rel_time, pred);
		}
	};

	/**
	 *	\class Notifiable Library/Threading/Condition.hpp
	 *	\brief A templated class to use when setting an object notifiable.
	 *	Can be useful for notifying object of the STL, like `Threading::Notifiable<std::list<int>>`.
	 */
	template<class C>
	class	Notifiable :public C, public Threading::Condition {
	public:
		/**
		 *	\brief Default constructor of Notifiable.
		 *	\params args the arguments which will be passed to the constructor of the templated class.
		 */
		template<typename... Args>
		Notifiable(const Args&... args):
			C(args...),
			Threading::Condition()
		{}

		/**
		 *	\brief Destructor of Notifiable.
		 */
		virtual ~Notifiable() {}
	};
}

#endif		/* __LIBRARY_THREADING_CONDITION_HPP__ */