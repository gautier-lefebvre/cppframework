#include	"Library/Threading/Lock.hpp"

Threading::Lock::Lock():
	_lock()
{}

Threading::Lock::~Lock() {}

void	Threading::Lock::lock() {
	this->_lock.lock();
}

void	Threading::Lock::unlock() {
	this->_lock.unlock();
}

Threading::ReadWriteLock::WriterGuard::WriterGuard(Threading::ReadWriteLock *lock):
	_lock(lock) {
	this->_lock->writerAcquire();
}

Threading::ReadWriteLock::WriterGuard::~WriterGuard() {
	this->_lock->writerRelease();
}

Threading::ReadWriteLock::ReaderGuard::ReaderGuard(Threading::ReadWriteLock *lock):
	_lock(lock) {
	this->_lock->readerAcquire();
}

Threading::ReadWriteLock::ReaderGuard::~ReaderGuard() {
	this->_lock->readerRelease();
}

Threading::ReadWriteLock::LightSwitch::LightSwitch():
	Threading::Lock(),
	_counter(0)
{}

void Threading::ReadWriteLock::LightSwitch::acquire(Threading::Lock& lock) {
	SCOPELOCK(this);
	this->_counter++;
	if (this->_counter == 1) {
		lock.lock();
	}
}

void Threading::ReadWriteLock::LightSwitch::release(Threading::Lock& lock) {
	SCOPELOCK(this);
	this->_counter--;
	if (this->_counter == 0) {
		lock.unlock();
	}
}

Threading::ReadWriteLock::ReadWriteLock():
	_readSwitch(),
	_writeSwitch(),
	_noReaders(),
	_noWriters(),
	_readersQueue()
{}

void	Threading::ReadWriteLock::readerAcquire() {
	ScopeLock slr(this->_readersQueue);
	{
		ScopeLock slw(this->_noReaders);
		this->_readSwitch.acquire(this->_noWriters);
	}
}

void	Threading::ReadWriteLock::readerRelease() {
	this->_readSwitch.release(this->_noWriters);
}

void	Threading::ReadWriteLock::writerAcquire() {
	this->_writeSwitch.acquire(this->_noReaders);
	this->_noWriters.lock();
}

void	Threading::ReadWriteLock::writerRelease() {
	this->_noWriters.unlock();
	this->_writeSwitch.release(this->_noReaders);
}
