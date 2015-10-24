#include  <signal.h>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/Manager.hh"
#include  "Core/Network/Exception.hh"

Core::Network::Manager::Manager(void):
  _input(),
  _output(),
  _tcp(_input, _output)
  // _udp(_input, _output)
{}

Core::Network::Manager::~Manager(void) {
  this->end();
}

void Core::Network::Manager::end(void) {
  SCOPELOCK(this);
  if (!(this->mustEnd())) {
    this->mustEnd(true);

    // wake both threads
    {
      ScopeLock sl(this->_output.condition);
      this->_output.condition.notify_all();
    }

    {
      ScopeLock sl(this->_input.condition);
      this->_input.condition.notify_all();
    }

    // wait for threads to end
    try {
      if (this->_input.thread) {
        this->_input.thread->join();
        delete this->_input.thread;
      }
      this->_input.thread = nullptr;
    } catch (const std::system_error& e) {
      WARNING(e.what());
    }

    try {
      if (this->_output.thread) {
        this->_output.thread->join();
        delete this->_output.thread;
      }
      this->_output.thread = nullptr;
    } catch (const std::system_error& e) {
      WARNING(e.what());
    }

    // close every TCP servers / connections
    this->_tcp.clear();
  }
}

void Core::Network::Manager::init(void) {
  // ignore SIGPIPE (recv and send will return error)
  signal(SIGPIPE, SIG_IGN);

  // create IO threads
  this->_input.thread  = new std::thread(&Core::Network::Manager::inputRoutine, this);
  this->_output.thread = new std::thread(&Core::Network::Manager::outputRoutine, this);
}

void Core::Network::Manager::inputRoutine(void) {
  fd_set   rset;
  int      rmax;
  timeval  clock;
  uint32_t nb;

  while (!(this->mustEnd())) {
    // reset
    FD_ZERO(&rset);
    rmax = 0;
    nb = 0;

    // fill set
    this->_tcp.fillSetRead(rset, rmax, nb);

    // if no socket to read on -> wait for a socket to be added
    if (!nb) {
      SCOPELOCK(&(this->_input.condition));
      this->_input.condition.wait();
      continue;
    }

    // wait for socket to be readable (1 sec timeout)
    clock.tv_sec  = 1;
    clock.tv_usec = 0;
    select(rmax + 1, &rset, nullptr, nullptr, &clock);

    if (this->mustEnd()) { break; }

    // read available sockets
    this->_tcp.recv(rset);
  }
}

void Core::Network::Manager::outputRoutine(void) {
  fd_set   wset;
  int      wmax;
  uint32_t nb;

  while (!(this->mustEnd())) {
    // reset
    FD_ZERO(&wset);
    wmax = 0;
    nb = 0;

    // fill set
    this->_tcp.fillSetWrite(wset, wmax, nb);

    // if no data to send -> wait for data to send
    if (!nb) {
      SCOPELOCK(&(this->_output.condition));
      this->_output.condition.wait();
      continue;
    }

    // wait for socket to be writable (no timeout)
    select(wmax + 1, nullptr, &wset, nullptr, nullptr);

    // write available sockets
    this->_tcp.send(wset);
  }
}
