#include  "Core/Network/Tcp/TcpEvents.hh"

using namespace fwk;

/**
 *  TcpSocketStreamEventArgs
 */

TcpSocketStreamEventArgs::TcpSocketStreamEventArgs(void):
  APooled<TcpSocketStreamEventArgs>(),
  socket(nullptr)
{}

void TcpSocketStreamEventArgs::reinit(void) {
  this->socket = nullptr;
}

void TcpSocketStreamEventArgs::init(TcpSocketStream* socket) {
  this->socket = socket;
}

void TcpSocketStreamEventArgs::cleanup(void) {
  TcpSocketStreamEventArgs::returnToPool(this);
}

/**
 *  TcpSocketEventArgs
 */

TcpSocketEventArgs::TcpSocketEventArgs(void):
  APooled<TcpSocketEventArgs>(),
  socket(nullptr)
{}

void TcpSocketEventArgs::reinit(void) {
  this->socket = nullptr;
}

void TcpSocketEventArgs::init(TcpSocket* socket) {
  this->socket = socket;
}

void TcpSocketEventArgs::cleanup(void) {
  TcpSocketEventArgs::returnToPool(this);
}
