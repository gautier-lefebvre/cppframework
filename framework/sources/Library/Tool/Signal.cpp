#include "Library/ThirdParty/cppformat/format.hh"
#include "Library/Tool/Logger.hpp"
#include "Library/Tool/Signal.hh"

const BidiMap<Signal::Type, int> Signal::SignalTypeToInt = {
  {Signal::Type::INT, SIGINT}
};

static void my_signal_handler(int signum) {
  try {
    Signal::get().handle(signum);
  } catch (const std::out_of_range&) {
    CRITICAL(fmt::format("The signal [signum = {0}] was caught but could not be handled. This is not supposed to happen.", signum));
  }
}

Signal::Signal(void):
  Threading::Lockable(),
  callbacks(),
  oldcallbacks()
{}

Signal::~Signal(void) {
  SCOPELOCK(this);
  for (auto& old : this->oldcallbacks) {
    delete old.second;
  }
}

void Signal::setCallback(Signal::Type type, const std::function<bool (void)>& callback) {
  SCOPELOCK(this);
  int signum = SignalTypeToInt.key.at(type);

  if (this->callbacks.find(signum) != this->callbacks.end()) {
    this->delCallback(type);
  }

  if (callback != nullptr) {
    struct sigaction sigHandler;
    struct sigaction *old = new struct sigaction();

    sigHandler.sa_handler = my_signal_handler;
    sigemptyset(&sigHandler.sa_mask);
    sigHandler.sa_flags = 0;
    sigaction(signum, &sigHandler, old);

    this->callbacks[signum] = callback;
    this->oldcallbacks[signum] = old;
  }
}

void Signal::delCallback(Signal::Type type) {
  SCOPELOCK(this);

  int signum = SignalTypeToInt.key.at(type);

  auto iterator = this->oldcallbacks.find(signum);
  if (iterator == this->oldcallbacks.end()) {
    throw std::out_of_range("This signal was not handled");
  } else {
    struct sigaction* old = (*iterator).second;
    sigaction(signum, old, nullptr);
    this->oldcallbacks.erase(iterator);

    if (old) {
      delete old;
    }
  }

  this->callbacks.erase(signum);
}

void Signal::handle(int signum) {
  SCOPELOCK(this);
  if (!this->callbacks.at(signum)()) {
    this->delCallback(Signal::SignalTypeToInt.value.at(signum));
  }
}
