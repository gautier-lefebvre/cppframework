#include  "Core/System.hh"
#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/Signal.hh"
#include  "Core/Network/Http/HttpClient.hh"
#include  "Core/Network/NetworkManager.hh"
#include  "Core/Worker/WorkerManager.hh"
#include  "Core/FactoryManager.hh"

using namespace fwk;

System::System(void):
  Lockable(),
  AEndable(),
  _endCondition(),
  _modulesInitialized()
{
  this->_modulesInitialized[System::Modules::HTTP] = false;
  this->_modulesInitialized[System::Modules::UDP] = false;
  this->_modulesInitialized[System::Modules::TCP] = false;
  this->_modulesInitialized[System::Modules::WORKERS] = false;

  FactoryManager::get().init();
}

System::~System(void) {
  this->cleanup();
}

void System::cleanup(void) {
  SCOPELOCK(this);

  // end modules
  this->endHTTP();
  this->endTCP();
  this->endUDP();
  this->endWorkerThreads();

  // destroy singletons
  HttpClient::destroy();
  NetworkManager::destroy();
  WorkerManager::destroy();
  Signal::destroy();

  // destroy core factory
  FactoryManager::get().end();
  FactoryManager::destroy();

  // destroy logger last
  LOGGER_DESTROY;
}

void System::onEnd(void) {
  SCOPELOCK(this);

  ScopeLock sl(this->_endCondition);
  this->_endCondition.notify_all();
}

void System::initHTTP(const std::string& userAgent) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(System::Modules::HTTP)) {
    HttpClient::get().init(userAgent);
    this->_modulesInitialized[System::Modules::HTTP] = true;
  }
}

void System::endHTTP(void) {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(System::Modules::HTTP)) {
    HttpClient::get().end();
    this->_modulesInitialized[System::Modules::HTTP] = false;
  }
}

void System::initTCP(void) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(System::Modules::TCP)) {
    NetworkManager::get().init();
    this->_modulesInitialized[System::Modules::TCP] = true;
  }
}

void System::endTCP(void) {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(System::Modules::TCP)) {
    NetworkManager::get().endTCP();
    this->_modulesInitialized[System::Modules::TCP] = false;

    // if UDP was not init or already ended -> end network manager
    if (!this->_modulesInitialized.at(System::Modules::UDP)) {
      NetworkManager::get().end();
    }
  }
}

void System::initUDP(void) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(System::Modules::UDP)) {
    NetworkManager::get().init();
    this->_modulesInitialized[System::Modules::UDP] = true;
  }
}

void System::endUDP(void) {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(System::Modules::UDP)) {
    NetworkManager::get().endUDP();
    this->_modulesInitialized[System::Modules::UDP] = false;

    // if TCP was not init or already ended -> end network manager
    if (!this->_modulesInitialized.at(System::Modules::TCP)) {
      NetworkManager::get().end();
    }
  }
}

void System::initWorkerThreads(size_t nbWorkerThreads, bool enableDelayedTasks) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(System::Modules::WORKERS)) {
    WorkerManager::get().init(nbWorkerThreads, enableDelayedTasks);
    this->_modulesInitialized[System::Modules::WORKERS] = true;
  }
}

void System::endWorkerThreads() {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(System::Modules::WORKERS)) {
    WorkerManager::get().end();
    this->_modulesInitialized[System::Modules::WORKERS] = false;
  }
}

void System::run(void) {
  // default config for workers (need at least 1 worker thread)
  this->initWorkerThreads(1, false);

  // wait for sigint
  SCOPELOCK(&(this->_endCondition));
  this->_endCondition.wait([this] (void) -> bool { return this->isEnding(); });
}
