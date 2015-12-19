#include  "Library/Collection/ByteArray.hpp"
#include  "Core/Network/HTTP/Request.hh"
#include  "Core/Network/HTTP/Response.hh"
#include  "Core/Network/TCP/Manager.hh"
#include  "Core/Network/TCP/Socket.hh"
#include  "Core/Network/TCP/SocketStream.hh"
#include  "Core/Network/UDP/SocketClient.hh"
#include  "Core/Network/UDP/SocketStream.hh"
#include  "Core/Network/UDP/SocketServer.hh"
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

    ByteArray::initPool("ByteArray");

    Core::Event::Event::initPool("Core::Event::Event");

    Core::Network::HTTP::Request::initPool("Core::Network::HTTP::Request");
    Core::Network::HTTP::Response::initPool("Core::Network::HTTP::Response");
    Core::Network::TCP::SocketStream::initPool("Core::Network::TCP::SocketStream");
    Core::Network::TCP::EventArgs::SocketArgs::initPool("Core::Network::TCP::EventArgs::SocketArgs");
    Core::Network::TCP::EventArgs::SocketStreamArgs::initPool("Core::Network::TCP::EventArgs::SocketStreamArgs");
    Core::Network::UDP::SocketClient::initPool("Core::Network::UDP::SocketClient");
    Core::Network::UDP::SocketStream::initPool("Core::Network::UDP::SocketStream");
    Core::Network::UDP::SocketServer::initPool("Core::Network::UDP::SocketServer");

    Core::Worker::EventTask::initPool("Core::Worker::EventTask");
    Core::Worker::HTTPTask::initPool("Core::Worker::HTTPTask");
    Core::Worker::PeriodicTask::initPool("Core::Worker::PeriodicTask");
    Core::Worker::DelayedTask::initPool("Core::Worker::DelayedTask");
  }
}

void Core::Factory::end(void) {
  SCOPELOCK(this);
  if (this->isInitialized()) {
    this->isInitialized(false);

    ByteArray::destroyPool();

    Core::Event::Event::destroyPool();

    Core::Network::HTTP::Request::destroyPool();
    Core::Network::HTTP::Response::destroyPool();
    Core::Network::TCP::SocketStream::destroyPool();
    Core::Network::TCP::EventArgs::SocketArgs::destroyPool();
    Core::Network::TCP::EventArgs::SocketStreamArgs::destroyPool();
    Core::Network::UDP::SocketClient::destroyPool();
    Core::Network::UDP::SocketStream::destroyPool();
    Core::Network::UDP::SocketServer::destroyPool();

    Core::Worker::EventTask::destroyPool();
    Core::Worker::HTTPTask::destroyPool();
    Core::Worker::PeriodicTask::destroyPool();
    Core::Worker::DelayedTask::destroyPool();
  }    
}
