#include  "Core/Network/HTTP/Response.hh"

Core::Network::HTTP::Response::Response(void):
  Core::Network::HTTP::AMessage(),
  status(0),
  reason("")
{}

Core::Network::HTTP::Response::~Response(void) {}

void  Core::Network::HTTP::Response::reinit(void) {
  this->Core::Network::HTTP::AMessage::reinit();
  this->status = 0;
  this->reason = "";
}
