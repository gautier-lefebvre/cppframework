#ifndef    __LIBRARY_THREADING_LOCK_HPP__
#define    __LIBRARY_THREADING_LOCK_HPP__

#include  <mutex>

/**
 *  \namespace Threading
 *  \brief Classes to use for synchronization, like locks and conditions variables.
 */
namespace   Threading {

  /**
   *  \class Lockable Library/Threading/Lock.hpp
   *  \brief A reentrant lock class.
   */
  class  Lockable {
  protected:
    std::recursive_mutex _lock; /*!< the mutex used for locking */

  public:
    /**
     *  \brief Constructor of Lockable.
     *  Inits the mutex.
     */
    Lockable(void);

    /**
     *  \brief Destructor of Lockable.
     */
    virtual ~Lockable(void);

  public:
    /**
     *  \brief Locks the mutex.
     */
    void  lock(void);

    /**
     *  \brief Unlocks the mutex.
     */
    void  unlock(void);
  };

  /**
   *  \class TLockable Library/Threading/Lock.hpp
   *  \brief A templated class to use when setting an object lockable.
   *  Can be useful for locking object of the STL, like `Threading::TLockable<std::list<int>>`.
   */
  template<class C>
  class  TLockable :public C, public Threading::Lockable {
  public:
    /**
     *  \brief Copy constructor of TLockable.
     *  \param oth the TLockable object to copy.
     */
    TLockable(const TLockable<C>& oth):
      C(oth),
      Threading::Lockable()
    {}

    /**
     *  \brief Default constructor of TLockable.
     *  \param args the arguments which will be passed to the constructor of the templated class.
     */
    template<typename... Args>
    TLockable(const Args&... args):
      C(args...),
      Threading::Lockable()
    {}

    /**
     *  \brief Destructor of TLockable.
     */
    virtual ~TLockable(void) {}
  };

  /**
   *  \class ReadWriteLock Library/Threading/Lock.hpp
   *  \brief A reader writer lock with writer priority.
   */
  class  ReadWriteLock {
  public:
    /**
     *  \class WriterGuard Library/Threading/Lock.hpp
     *  \brief A guard class which locks a ReadWriteLock as a writer in its constructor and unlocks it in its destructor.
     */
    class WriterGuard {
    private:
      ReadWriteLock *_lock; /*!< the read write lock. */

    public:
      /**
       *  \brief Constructor of WriterGuard. Locks the lock passed in parameter as writer.
       *  \param lock the lock.
       */
      WriterGuard(ReadWriteLock* lock);

      /**
       *  \brief Destructor of WriterGuard. Unlocks the lock given at creation as writer.
       */
      ~WriterGuard(void);
    };

    /**
     *  \class ReaderGuard Library/Threading/Lock.hpp
     *  \brief A guard class which locks a ReadWriteLock as a reader in its constructor and unlocks it in its destructor.
     */
    class ReaderGuard {
    private:
      ReadWriteLock *_lock; /*!< the read write lock. */

    public:
      /**
       *  \brief Constructor of ReaderGuard. Locks the lock passed in parameter as reader.
       *  \param lock the lock.
       */
      ReaderGuard(ReadWriteLock* lock);

      /**
       *  \brief Destructor of ReaderGuard. Unlocks the lock given at creation as reader.
       */
       ~ReaderGuard(void);
    };

  private:
    /**
     *  \class LightSwitch Library/Threading/Lock.hpp
     *  \brief an object used to count the number of times another mutex was locked.
     */
    class  LightSwitch {
    private:
      std::mutex _mutex;
      size_t     _counter; /*!< the number of times the mutex was locked. */

    public:
      /**
       *  \brief Constructor of LightSwitch. Sets the counter to 0.
       */
      LightSwitch(void);

    public:
      /**
       *  \brief Increments the counter and locks the mutex if it hits 1.
       *  \param mutex the mutex to lock.
       */
      void  acquire(std::mutex& mutex);

      /**
       *  \brief Decrements the counter and unlocks the mutex if it hits 0.
       *  \param mutex the mutex to lock.
       */
      void  release(std::mutex& mutex);
    };

  private:
    LightSwitch  _readSwitch; /*!< the switch used for the _noWriters mutex. */
    LightSwitch  _writeSwitch; /*!< the switch used for the _noReaders mutex. */
    std::mutex  _noReaders; /*!< the mutex used to check if there are no readers. */
    std::mutex  _noWriters; /*!< the mutex used to check if there are no writers. */
    std::mutex  _readersQueue; /*!< the mutex used to make readers wait to lock the ReadWriteLock. */

  public:
    /**
     *  \brief Constructor of ReadWriteLock.
     */
    ReadWriteLock(void);

    /**
     *  \brief Locks the ReadWriteLock as a reader.
     */
    void  readerAcquire(void);

    /**
     *  \brief Unlocks the ReadWriteLock as a reader.
     */
    void  readerRelease(void);

    /**
     *  \brief Locks the ReadWriteLock as a writer.
     */
    void  writerAcquire(void);

    /**
     *  \brief Unlocks the ReadWriteLock as a writer.
     */
    void  writerRelease(void);
  };
}

typedef std::lock_guard<Threading::Lockable> ScopeLock;
typedef std::lock_guard<std::mutex>          ScopeLockMutex;

#define SCOPELOCK(x)       std::lock_guard<Threading::Lockable> lockguard(*(x));
#define SCOPELOCK_MUTEX(x) std::lock_guard<std::mutex>          lockguard_mutex(x);

#endif    /* __LIBRARY_THREADING_LOCK_HPP__ */
