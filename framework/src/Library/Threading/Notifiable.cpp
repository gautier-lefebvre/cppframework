#include  "Library/Threading/Notifiable.hpp"

using namespace fwk;

Notifiable::Notifiable(void):
    Lockable(),
    _condvar()
{}

Notifiable::~Notifiable(void) {}

void  Notifiable::wait(void) {
    this->_condvar.wait(*this);
}

void  Notifiable::notify(void) {
    this->_condvar.notify_one();
}

void  Notifiable::notify_all(void) {
    this->_condvar.notify_all();
}