#include  "Core/Factory.hh"
#include  "Library/Collection/ByteArray.hpp"
#include  "Core/Network/HTTP/Request.hh"
#include  "Core/Network/HTTP/Response.hh"
#include  "Core/Network/TCP/Socket.hh"
#include  "Core/Network/TCP/SocketStream.hh"
#include  "Core/Network/UDP/SocketClient.hh"
#include  "Core/Worker/Task.hh"

void Core::Factory::init(void) {
  ByteArray::initPool("ByteArray");

  Core::Network::HTTP::Request::initPool("Core::Network::HTTP::Request");
  Core::Network::HTTP::Response::initPool("Core::Network::HTTP::Response");
  Core::Network::TCP::Socket::initPool("Core::Network::TCP::Socket");
  Core::Network::TCP::SocketStream::initPool("Core::Network::TCP::SocketStream");
  Core::Network::UDP::SocketClient::initPool("Core::Network::TCP::SocketClient");

  Core::Worker::EventTask::initPool("Core::Worker::EventTask");
  Core::Worker::HTTPTask::initPool("Core::Worker::HTTPTask");
  Core::Worker::PeriodicTask::initPool("Core::Worker::PeriodicTask");
  Core::Worker::DelayedTask::initPool("Core::Worker::DelayedTask");
}

void Core::Factory::end(void) {
  ByteArray::destroyPool();

  Core::Network::HTTP::Request::destroyPool();
  Core::Network::HTTP::Response::destroyPool();
  Core::Network::TCP::Socket::destroyPool();
  Core::Network::TCP::SocketStream::destroyPool();
  Core::Network::UDP::SocketClient::destroyPool();

  Core::Worker::EventTask::destroyPool();
  Core::Worker::HTTPTask::destroyPool();
  Core::Worker::PeriodicTask::destroyPool();
  Core::Worker::DelayedTask::destroyPool();
}