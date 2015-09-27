#ifndef		__LIBRARY_THREADING_LOCK_HH__
#define		__LIBRARY_THREADING_LOCK_HH__

#include	<mutex>

namespace 	Threading {
	class	Lock {
	protected:
		std::recursive_mutex _lock;

	public:
		Lock(void);
		virtual ~Lock();

		void	lock();
		void	unlock();
	};

	template<class C>
	class	Lockable :public C, public Threading::Lock {
	public:
		Lockable(const Lockable<C>& oth):
			C(oth),
			Threading::Lock()
		{}

		template<typename... Args>
		Lockable(const Args&... args):
			C(args...),
			Threading::Lock()
		{}

		virtual ~Lockable() {}
	};

	class	ReadWriteLock {
	public:
		class WriterGuard {
		private:
			ReadWriteLock *_lock;

		public:
			WriterGuard(ReadWriteLock*);
			~WriterGuard();
		};

		class ReaderGuard {
		private:
			ReadWriteLock *_lock;

		public:
			ReaderGuard(ReadWriteLock*);
			~ReaderGuard();
		};

	private:
		class  LightSwitch :public Threading::Lock {
		private:
			size_t	_counter;

		public:
			LightSwitch();

		public:
			void	acquire(Lock&);
			void	release(Lock&);
		};

	private:
		LightSwitch	_readSwitch;
		LightSwitch	_writeSwitch;
		Lock		_noReaders;
		Lock		_noWriters;
		Lock		_readersQueue;

	public:
		ReadWriteLock();

		void	readerAcquire();
		void	readerRelease();

		void	writerAcquire();
		void	writerRelease();
	};
}

typedef		std::lock_guard<Threading::Lock> ScopeLock;

#define		SCOPELOCK(x)		std::lock_guard<Threading::Lock> lockguard(*(x));

#endif		/* __LIBRARY_THREADING_LOCK_HH__ */