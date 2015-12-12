#include  "Core/System.hh"
#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/HTTP/Client.hh"
#include  "Core/Network/Manager.hh"
#include  "Core/Worker/Manager.hh"
#include  "Core/Factory.hh"

Core::System::System(void):
  Threading::Lockable(),
  AEndable(),
  _endCondition(),
  _modulesInitialized()
{
  this->_modulesInitialized[Core::System::Modules::HTTP] = false;
  this->_modulesInitialized[Core::System::Modules::UDP] = false;
  this->_modulesInitialized[Core::System::Modules::TCP] = false;
  this->_modulesInitialized[Core::System::Modules::WORKERS] = false;
  this->_modulesInitialized[Core::System::Modules::LOGGER] = false;

  Core::Factory::get().init();
}

Core::System::~System(void) {
  this->cleanup();
}

void Core::System::cleanup(void) {
  SCOPELOCK(this);

  // end modules
  this->endHTTP();
  this->endTCP();
  this->endUDP();
  this->endWorkerThreads();

  // destroy singletons
  Core::Network::HTTP::Client::destroy();
  Core::Network::Manager::destroy();
  Core::Worker::Manager::destroy();
  LOGGER_DESTROY();

  // destroy core factory
  Core::Factory::get().end();
  Core::Factory::destroy();
}

void Core::System::end(void) {
  SCOPELOCK(this);
  if (!this->mustEnd()) {
    ScopeLock sl(this->_endCondition);
    this->mustEnd(true);
    this->_endCondition.notify_all();
  }
}

void Core::System::initHTTP(const std::string& userAgent) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(Core::System::Modules::HTTP)) {
    Core::Network::HTTP::Client::get().init(userAgent);
    this->_modulesInitialized[Core::System::Modules::HTTP] = true;
  }
}

void Core::System::endHTTP(void) {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(Core::System::Modules::HTTP)) {
    Core::Network::HTTP::Client::get().end();
    this->_modulesInitialized[Core::System::Modules::HTTP] = false;
  }
}

void Core::System::initTCP(void) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(Core::System::Modules::TCP)) {
    Core::Network::Manager::get().init();
    this->_modulesInitialized[Core::System::Modules::TCP] = true;
  }
}

void Core::System::endTCP(void) {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(Core::System::Modules::TCP)) {
    Core::Network::Manager::get().endTCP();
    this->_modulesInitialized[Core::System::Modules::TCP] = false;

    // if UDP was not init or already ended -> end network manager
    if (!this->_modulesInitialized.at(Core::System::Modules::UDP)) {
      Core::Network::Manager::get().end();
    }
  }
}

void Core::System::initUDP(void) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(Core::System::Modules::UDP)) {
    Core::Network::Manager::get().init();
    this->_modulesInitialized[Core::System::Modules::UDP] = true;
  }
}

void Core::System::endUDP(void) {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(Core::System::Modules::UDP)) {
    Core::Network::Manager::get().endUDP();
    this->_modulesInitialized[Core::System::Modules::TCP] = false;

    // if TCP was not init or already ended -> end network manager
    if (!this->_modulesInitialized.at(Core::System::Modules::TCP)) {
      Core::Network::Manager::get().end();
    }
  }
}

void Core::System::initWorkerThreads(size_t nbWorkerThreads, bool enableDelayedTasks) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(Core::System::Modules::WORKERS)) {
    Core::Worker::Manager::get().init(nbWorkerThreads, enableDelayedTasks);
    this->_modulesInitialized[Core::System::Modules::WORKERS] = true;
  }
}

void Core::System::endWorkerThreads() {
  SCOPELOCK(this);
  if (this->_modulesInitialized.at(Core::System::Modules::WORKERS)) {
    Core::Worker::Manager::get().end();
    this->_modulesInitialized[Core::System::Modules::WORKERS] = false;
  }
}

void Core::System::initLogger(Logger::Level level) {
  SCOPELOCK(this);
  if (!this->_modulesInitialized.at(Core::System::Modules::UDP)) {
    LOGGER_SET_LEVEL(level);
    this->_modulesInitialized[Core::System::Modules::LOGGER] = true;
  }
}

void Core::System::run(void) {
  // default config for workers (need at least 1 worker thread)
  this->initWorkers(1, false);

  // wait for sigint
  SCOPELOCK(&(this->_endCondition));
  this->_endCondition.wait([this] () -> bool { return this->mustEnd(); });
}
