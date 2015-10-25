#include  "Core/Network/HTTP/Request.hh"

Core::Network::HTTP::Request::Request(void):
  Core::Network::HTTP::AMessage(),
  method(""),
  url(""),
  secure(false),
  success(nullptr),
  error(nullptr),
  clean(nullptr),
  file({false, ""})
{}

Core::Network::HTTP::Request::~Request(void) {}

void  Core::Network::HTTP::Request::reinit(void) {
  this->Core::Network::HTTP::AMessage::reinit();
  this->method  = "";
  this->url     = "";
  this->secure  = false;
  this->success = nullptr;
  this->error   = nullptr;
  this->clean   = nullptr;
  this->file.isFile = false;
  this->file.filepath = "";
}
