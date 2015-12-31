#include  "Library/Collection/ByteArray.hpp"
#include  "Core/Network/HTTP/Request.hh"
#include  "Core/Network/HTTP/Response.hh"
#include  "Core/Network/TCP/Manager.hh"
#include  "Core/Network/UDP/Manager.hh"
#include  "Core/Worker/Task.hh"
#include  "Core/Factory.hh"

Core::Factory::Factory(void):
  Initializable(),
  Threading::Lockable()
{}

Core::Factory::~Factory(void) {
  this->end();
}

void Core::Factory::init(void) {
  SCOPELOCK(this);
  if (!this->isInitialized()) {
    this->isInitialized(true);

    // Library
    ByteArray::initPool("ByteArray");

    // Core
    // -- Events
    Core::Event::Event::initPool("Core::Event::Event");

    // -- HTTP
    Core::Network::HTTP::Request::initPool("Core::Network::HTTP::Request");
    Core::Network::HTTP::Response::initPool("Core::Network::HTTP::Response");

    // -- TCP
    Core::Network::TCP::SocketStream::initPool("Core::Network::TCP::SocketStream");
    // ---- TCP Events
    Core::Network::TCP::EventArgs::SocketArgs::initPool("Core::Network::TCP::EventArgs::SocketArgs");
    Core::Network::TCP::EventArgs::SocketStreamArgs::initPool("Core::Network::TCP::EventArgs::SocketStreamArgs");

    // -- UDP
    Core::Network::UDP::SocketClient::initPool("Core::Network::UDP::SocketClient");
    Core::Network::UDP::SocketStream::initPool("Core::Network::UDP::SocketStream");
    Core::Network::UDP::SocketServer::initPool("Core::Network::UDP::SocketServer");
    // -- UDP Events
    Core::Network::UDP::EventArgs::SocketStreamArgs::initPool("Core::Network::UDP::EventArgs::SocketStreamArgs");
    Core::Network::UDP::EventArgs::SocketServerArgs::initPool("Core::Network::UDP::EventArgs::SocketServerArgs");
    Core::Network::UDP::EventArgs::SocketClientArgs::initPool("Core::Network::UDP::EventArgs::SocketClientArgs");

    // -- Tasks
    Core::SimpleTask::initPool("Core::SimpleTask");
    Core::EventTask::initPool("Core::EventTask");
    Core::HTTPTask::initPool("Core::HTTPTask");
    Core::PeriodicTask::initPool("Core::PeriodicTask");
    Core::DelayedTask::initPool("Core::DelayedTask");
  }
}

void Core::Factory::end(void) {
  SCOPELOCK(this);
  if (this->isInitialized()) {
    this->isInitialized(false);

    // Library
    ByteArray::destroyPool();

    // Core
    // -- Events
    Core::Event::Event::destroyPool();

    // -- HTTP
    Core::Network::HTTP::Request::destroyPool();
    Core::Network::HTTP::Response::destroyPool();

    // -- TCP
    Core::Network::TCP::SocketStream::destroyPool();
    // ---- TCP Events
    Core::Network::TCP::EventArgs::SocketArgs::destroyPool();
    Core::Network::TCP::EventArgs::SocketStreamArgs::destroyPool();

    // -- UDP
    Core::Network::UDP::SocketClient::destroyPool();
    Core::Network::UDP::SocketStream::destroyPool();
    Core::Network::UDP::SocketServer::destroyPool();
    // ---- UDP Events
    Core::Network::UDP::EventArgs::SocketStreamArgs::destroyPool();
    Core::Network::UDP::EventArgs::SocketServerArgs::destroyPool();
    Core::Network::UDP::EventArgs::SocketClientArgs::destroyPool();

    // -- Tasks
    Core::SimpleTask::destroyPool();
    Core::EventTask::destroyPool();
    Core::HTTPTask::destroyPool();
    Core::PeriodicTask::destroyPool();
    Core::DelayedTask::destroyPool();
  }    
}
