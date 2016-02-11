#include  "Library/Threading/Lockable.hpp"
#include  "Library/Tool/Logger.hpp"

using namespace fwk;

Lockable::Lockable(void):
  _lock()
{}

Lockable::~Lockable(void) {}

void  Lockable::lock(void) {
  this->_lock.lock();
}

void  Lockable::unlock(void) {
  this->_lock.unlock();
}

ReadWriteLock::WriterGuard::WriterGuard(ReadWriteLock *lock):
  _lock(lock) {
  this->_lock->writerAcquire();
}

ReadWriteLock::WriterGuard::~WriterGuard(void) {
  this->_lock->writerRelease();
}

ReadWriteLock::ReaderGuard::ReaderGuard(ReadWriteLock *lock):
  _lock(lock) {
  this->_lock->readerAcquire();
}

ReadWriteLock::ReaderGuard::~ReaderGuard(void) {
  this->_lock->readerRelease();
}

ReadWriteLock::LightSwitch::LightSwitch(void):
  _mutex(),
  _counter(0)
{}

void ReadWriteLock::LightSwitch::acquire(std::mutex& lock) {
  SCOPELOCK_MUTEX(this->_mutex);
  this->_counter++;
  if (this->_counter == 1) {
    lock.lock();
  }
}

void ReadWriteLock::LightSwitch::release(std::mutex& lock) {
  SCOPELOCK_MUTEX(this->_mutex);
  this->_counter--;
  if (this->_counter == 0) {
    lock.unlock();
  }
}

ReadWriteLock::ReadWriteLock(void):
  _readSwitch(),
  _writeSwitch(),
  _noReaders(),
  _noWriters(),
  _readersQueue()
{}

void  ReadWriteLock::readerAcquire(void) {
  ScopeLockMutex slr(this->_readersQueue);
  {
    ScopeLockMutex slw(this->_noReaders);
    this->_readSwitch.acquire(this->_noWriters);
  }
}

void  ReadWriteLock::readerRelease(void) {
  this->_readSwitch.release(this->_noWriters);
}

void  ReadWriteLock::writerAcquire(void) {
  this->_writeSwitch.acquire(this->_noReaders);
  this->_noWriters.lock();
}

void  ReadWriteLock::writerRelease(void) {
  this->_noWriters.unlock();
  this->_writeSwitch.release(this->_noReaders);
}
