#include  "Library/Collection/ByteArray.hpp"
#include  "Library/Network/CURL/EasyHandle.hh"
#include  "Core/Network/Http/HttpRequest.hh"
#include  "Core/Network/Http/HttpResponse.hh"
#include  "Core/Network/Tcp/TcpManager.hh"
#include  "Core/Network/Udp/UdpManager.hh"
#include  "Core/Worker/Task.hh"
#include  "Core/FactoryManager.hh"

using namespace fwk;

FactoryManager::FactoryManager(void):
  Initializable(),
  Threading::Lockable()
{}

FactoryManager::~FactoryManager(void) {
  this->end();
}

void FactoryManager::init(void) {
  SCOPELOCK(this);
  if (!this->isInitialized()) {
    this->isInitialized(true);

    // Library
    ByteArray::initPool("ByteArray");
    curlxx::EasyHandle::initPool("curlxx::EasyHandle");

    // Core
    // -- Events
    EventHandle::initPool("EventHandle");

    // -- HTTP
    HttpRequest::initPool("HttpRequest");
    HttpResponse::initPool("HttpResponse");

    // -- TCP
    TcpSocketStream::initPool("TcpSocketStream");
    // ---- TCP Events
    TcpSocketEventArgs::initPool("TcpSocketEventArgs");
    TcpSocketStreamEventArgs::initPool("TcpSocketStreamEventArgs");

    // -- UDP
    UdpSocketClient::initPool("UdpSocketClient");
    UdpSocketStream::initPool("UdpSocketStream");
    UdpSocketServer::initPool("UdpSocketServer");
    // -- UDP Events
    UdpSocketStreamEventArgs::initPool("UdpSocketStreamEventArgs");
    UdpSocketServerEventArgs::initPool("UdpSocketServerEventArgs");
    UdpSocketClientEventArgs::initPool("UdpSocketClientEventArgs");

    // -- Tasks
    SimpleTask::initPool("SimpleTask");
    EventTask::initPool("EventTask");
    HttpTask::initPool("HttpTask");
    PeriodicTask::initPool("PeriodicTask");
    DelayedTask::initPool("DelayedTask");
  }
}

void FactoryManager::end(void) {
  SCOPELOCK(this);
  if (this->isInitialized()) {
    this->isInitialized(false);

    // Library
    ByteArray::destroyPool();
    curlxx::EasyHandle::destroyPool();

    // Core
    // -- Events
    EventHandle::destroyPool();

    // -- HTTP
    HttpRequest::destroyPool();
    HttpResponse::destroyPool();

    // -- TCP
    TcpSocketStream::destroyPool();
    // ---- TCP Events
    TcpSocketEventArgs::destroyPool();
    TcpSocketStreamEventArgs::destroyPool();

    // -- UDP
    UdpSocketClient::destroyPool();
    UdpSocketStream::destroyPool();
    UdpSocketServer::destroyPool();
    // ---- UDP Events
    UdpSocketStreamEventArgs::destroyPool();
    UdpSocketServerEventArgs::destroyPool();
    UdpSocketClientEventArgs::destroyPool();

    // -- Tasks
    SimpleTask::destroyPool();
    EventTask::destroyPool();
    HttpTask::destroyPool();
    PeriodicTask::destroyPool();
    DelayedTask::destroyPool();
  }    
}
