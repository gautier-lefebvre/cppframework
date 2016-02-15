#include  <signal.h>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/NetworkManager.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

NetworkManager::NetworkManager(void):
  Lockable(),
  AEndable(),
  Initializable(),
  _input(),
  _output(),
  _tcp(_input, _output),
  _udp(_input, _output)
{}

NetworkManager::~NetworkManager(void) {
  this->end();
}

void NetworkManager::onEnd(void) {
  SCOPELOCK(this);

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
  // close every UDP servers / connections
  this->_udp.clear();
}

void NetworkManager::init(void) {
  if (!this->isInitialized()) {
    // only execute this once.
    this->isInitialized(false);

    // ignore SIGPIPE (recv and send will return error)
    signal(SIGPIPE, SIG_IGN);

    // create IO threads
    this->_input.thread  = new std::thread(&NetworkManager::inputRoutine, this);
    this->_output.thread = new std::thread(&NetworkManager::outputRoutine, this);
  }
}

void NetworkManager::endTCP(void) {
  this->_tcp.clear();
}

void NetworkManager::endUDP(void) {
  this->_udp.clear();
}

void NetworkManager::inputRoutine(void) {
  fd_set   rset;
  int      rmax;
  timeval  clock;
  uint32_t nb;

  while (!(this->isEnding())) {
    // reset
    FD_ZERO(&rset);
    rmax = 0;
    nb = 0;

    // fill set
    this->_tcp.fillSetRead(rset, rmax, nb);
    this->_udp.fillSetRead(rset, rmax, nb);

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

    if (this->isEnding()) { break; }

    // read available sockets
    this->_tcp.recv(rset);
    this->_udp.recv(rset);
  }
}

void NetworkManager::outputRoutine(void) {
  fd_set   wset;
  int      wmax;
  uint32_t nb;

  while (!(this->isEnding())) {
    // reset
    FD_ZERO(&wset);
    wmax = 0;
    nb = 0;

    // fill set
    this->_tcp.fillSetWrite(wset, wmax, nb);
    this->_udp.fillSetWrite(wset, wmax, nb);

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
    this->_udp.send(wset);
  }
}

TcpManager& NetworkManager::getTCP(void) { return this->_tcp; }
UdpManager& NetworkManager::getUDP(void) { return this->_udp; }
