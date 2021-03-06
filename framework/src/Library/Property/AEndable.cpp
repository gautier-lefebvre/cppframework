#include  "Library/Property/AEndable.hh"

using namespace fwk;

AEndable::AEndable(void):
  _end(false),
  _endLock()
{}

AEndable::~AEndable(void) {}

void  AEndable::end(void) {
  bool callOnEnd = false;

  {
    SCOPELOCK(&(this->_endLock));
    if (!(this->isEnding())) {
      this->isEnding(true);
      callOnEnd = true;
    }
  }

  if (callOnEnd) {
    this->onEnd();
  }
}

bool  AEndable::isEnding(void) const {
  return this->_end;
}

void  AEndable::isEnding(bool state) {
  this->_end = state;
}
