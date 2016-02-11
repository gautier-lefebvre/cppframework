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
  Lockable()
{}

FactoryManager::~FactoryManager(void) {
  this->end();
}

void FactoryManager::init(void) {
  SCOPELOCK(this);
  if (!this->isInitialized()) {
    this->isInitialized(true);

    // Library
    ByteArray::initPool(100, 10, "ByteArray");
    curlxx::EasyHandle::initPool(1, 1, "curlxx::EasyHandle");

    // Core
    // -- Events
    EventHandle::initPool(50, 20, "EventHandle");

    // -- HTTP
    HttpRequest::initPool(20, 10, "HttpRequest");
    HttpResponse::initPool(20, 10, "HttpResponse");

    // -- TCP
    TcpSocketStream::initPool(20, 10, "TcpSocketStream");
    // ---- TCP Events
    TcpSocketEventArgs::initPool(10, 2, "TcpSocketEventArgs");
    TcpSocketStreamEventArgs::initPool(40, 20, "TcpSocketStreamEventArgs");

    // -- UDP
    UdpSocketClient::initPool(20, 10, "UdpSocketClient");
    UdpSocketStream::initPool(20, 10, "UdpSocketStream");
    UdpSocketServer::initPool(20, 10, "UdpSocketServer");
    // -- UDP Events
    UdpSocketStreamEventArgs::initPool(40, 20, "UdpSocketStreamEventArgs");
    UdpSocketServerEventArgs::initPool(40, 20, "UdpSocketServerEventArgs");
    UdpSocketClientEventArgs::initPool(40, 20, "UdpSocketClientEventArgs");

    // -- Tasks
    SimpleTask::initPool(40, 20, "SimpleTask");
    EventTask::initPool(200, 40, "EventTask");
    HttpTask::initPool(20, 10, "HttpTask");
    PeriodicTask::initPool(20, 10, "PeriodicTask");
    DelayedTask::initPool(40, 20, "DelayedTask");
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
