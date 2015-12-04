#include  "Library/Collection/ByteArray.hpp"
#include  "Core/Network/HTTP/Request.hh"
#include  "Core/Network/HTTP/Response.hh"
#include  "Core/Network/TCP/Socket.hh"
#include  "Core/Network/TCP/SocketStream.hh"
#include  "Core/Network/UDP/SocketClient.hh"
#include  "Core/Network/UDP/SocketStream.hh"
#include  "Core/Network/UDP/SocketServer.hh"
#include  "Core/Worker/Task.hh"
#include  "Core/Factory.hh"

void Core::Factory::init(void) {
  ByteArray::initPool("ByteArray");

  Core::Network::HTTP::Request::initPool("Core::Network::HTTP::Request");
  Core::Network::HTTP::Response::initPool("Core::Network::HTTP::Response");
  Core::Network::TCP::SocketStream::initPool("Core::Network::TCP::SocketStream");
  Core::Network::UDP::SocketClient::initPool("Core::Network::UDP::SocketClient");
  Core::Network::UDP::SocketStream::initPool("Core::Network::UDP::SocketStream");
  Core::Network::UDP::SocketServer::initPool("Core::Network::UDP::SocketServer");

  Core::Worker::EventTask::initPool("Core::Worker::EventTask");
  Core::Worker::HTTPTask::initPool("Core::Worker::HTTPTask");
  Core::Worker::PeriodicTask::initPool("Core::Worker::PeriodicTask");
  Core::Worker::DelayedTask::initPool("Core::Worker::DelayedTask");
}

void Core::Factory::end(void) {
  ByteArray::destroyPool();

  Core::Network::HTTP::Request::destroyPool();
  Core::Network::HTTP::Response::destroyPool();
  Core::Network::TCP::SocketStream::destroyPool();
  Core::Network::UDP::SocketClient::destroyPool();
  Core::Network::UDP::SocketStream::destroyPool();
  Core::Network::UDP::SocketServer::destroyPool();

  Core::Worker::EventTask::destroyPool();
  Core::Worker::HTTPTask::destroyPool();
  Core::Worker::PeriodicTask::destroyPool();
  Core::Worker::DelayedTask::destroyPool();
}
