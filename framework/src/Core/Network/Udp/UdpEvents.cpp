#include  "Core/Network/Udp/UdpEvents.hh"

using namespace fwk;

/**
 *  UdpSocketStreamEventArgs
 */

UdpSocketStreamEventArgs::UdpSocketStreamEventArgs(void):
  APooled<UdpSocketStreamEventArgs>(),
  socket(nullptr)
{}

void UdpSocketStreamEventArgs::reinit(void) {
  this->socket = nullptr;
}

void UdpSocketStreamEventArgs::init(UdpSocketStream* socket) {
  this->socket = socket;
}

void UdpSocketStreamEventArgs::cleanup(void) {
  UdpSocketStreamEventArgs::returnToPool(this);
}

/**
 *  UdpSocketServerEventArgs
 */

UdpSocketServerEventArgs::UdpSocketServerEventArgs(void):
  APooled<UdpSocketServerEventArgs>(),
  socket(nullptr)
{}

void UdpSocketServerEventArgs::reinit(void) {
  this->socket = nullptr;
}

void UdpSocketServerEventArgs::init(UdpSocketServer* socket) {
  this->socket = socket;
}

void UdpSocketServerEventArgs::cleanup(void) {
  UdpSocketServerEventArgs::returnToPool(this);
}

/**
 *  UdpSocketClientEventArgs
 */

UdpSocketClientEventArgs::UdpSocketClientEventArgs(void):
  APooled<UdpSocketClientEventArgs>(),
  socket(nullptr)
{}

void UdpSocketClientEventArgs::reinit(void) {
  this->socket = nullptr;
}

void UdpSocketClientEventArgs::init(UdpSocketClient* socket) {
  this->socket = socket;
}

void UdpSocketClientEventArgs::cleanup(void) {
  UdpSocketClientEventArgs::returnToPool(this);
}
