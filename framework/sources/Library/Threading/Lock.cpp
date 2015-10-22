#include  "Library/Threading/Lock.hpp"

Threading::Lock::Lock(void):
  _lock()
{}

Threading::Lock::~Lock(void) {}

void  Threading::Lock::lock(void) {
  this->_lock.lock();
}

void  Threading::Lock::unlock(void) {
  this->_lock.unlock();
}

Threading::ReadWriteLock::WriterGuard::WriterGuard(Threading::ReadWriteLock *lock):
  _lock(lock) {
  this->_lock->writerAcquire();
}

Threading::ReadWriteLock::WriterGuard::~WriterGuard(void) {
  this->_lock->writerRelease();
}

Threading::ReadWriteLock::ReaderGuard::ReaderGuard(Threading::ReadWriteLock *lock):
  _lock(lock) {
  this->_lock->readerAcquire();
}

Threading::ReadWriteLock::ReaderGuard::~ReaderGuard(void) {
  this->_lock->readerRelease();
}

Threading::ReadWriteLock::LightSwitch::LightSwitch(void):
  _mutex(),
  _counter(0)
{}

void Threading::ReadWriteLock::LightSwitch::acquire(std::mutex& lock) {
  SCOPELOCK_MUTEX(this->_mutex);
  this->_counter++;
  if (this->_counter == 1) {
    lock.lock();
  }
}

void Threading::ReadWriteLock::LightSwitch::release(std::mutex& lock) {
  SCOPELOCK_MUTEX(this->_mutex);
  this->_counter--;
  if (this->_counter == 0) {
    lock.unlock();
  }
}

Threading::ReadWriteLock::ReadWriteLock(void):
  _readSwitch(),
  _writeSwitch(),
  _noReaders(),
  _noWriters(),
  _readersQueue()
{}

void  Threading::ReadWriteLock::readerAcquire(void) {
  ScopeLockMutex slr(this->_readersQueue);
  {
    ScopeLockMutex slw(this->_noReaders);
    this->_readSwitch.acquire(this->_noWriters);
  }
}

void  Threading::ReadWriteLock::readerRelease(void) {
  this->_readSwitch.release(this->_noWriters);
}

void  Threading::ReadWriteLock::writerAcquire(void) {
  this->_writeSwitch.acquire(this->_noReaders);
  this->_noWriters.lock();
}

void  Threading::ReadWriteLock::writerRelease(void) {
  this->_noWriters.unlock();
  this->_writeSwitch.release(this->_noReaders);
}
