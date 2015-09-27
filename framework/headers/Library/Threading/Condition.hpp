#ifndef		__LIBRARY_THREADING_CONDITION_HPP__
#define		__LIBRARY_THREADING_CONDITION_HPP__

#include	<condition_variable>
#include	<chrono>

#include	"Library/Threading/Lock.hpp"

namespace	Threading {
	class	Condition :public Threading::Lock {
	private:
		std::condition_variable_any _condvar;

	public:
		Condition();
		virtual ~Condition();

		void	wait();
		void	notify();
		void	notify_all();

		template <class Predicate>
		void wait (Predicate pred) {
			this->_condvar.wait(*this, pred);
		}

		template<class C, class D>
		std::cv_status	wait_until(const std::chrono::time_point<C, D>& tp) {
			return this->_condvar.wait_until(*this, tp);
		}

		template<class C, class D, class P>
		bool wait_until(const std::chrono::time_point<C, D>& timeout_time, P pred) {
			return this->_condvar(*this, timeout_time, pred);
		}

		template<class R, class P>
		std::cv_status	wait_for(const std::chrono::duration<R, P>& rl) {
			return this->_condvar.wait_for(*this, rl);
		}

		template<class R, class Per, class Pre>
		bool wait_for(const std::chrono::duration<R, Per>& rel_time, Pre pred) {
			return this->_condvar.wait_for(*this, rel_time, pred);
		}
	};

	template<class C>
	class	Notifiable :public C, public Threading::Condition {
	public:
		template<typename... Args>
		Notifiable(const Args&... args):
			C(args...),
			Threading::Condition()
		{}

		virtual ~Notifiable() {}
	};
}

#endif		/* __LIBRARY_THREADING_CONDITION_HPP__ */