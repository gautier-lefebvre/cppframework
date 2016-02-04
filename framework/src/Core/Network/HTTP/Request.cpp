#include  "Core/Network/HTTP/Request.hh"

using namespace fwk;

Core::Network::HTTP::Request::Request(void):
  Core::Network::HTTP::AMessage(),
  method(""),
  url(""),
  success(nullptr),
  error(nullptr),
  clean(nullptr),
  file({false, ""}),
  asynchronous()
{
  this->asynchronous.isAsynchronous = true;
  this->asynchronous.isValid = false;
  this->asynchronous.response = nullptr;
}

Core::Network::HTTP::Request::~Request(void) {}

void  Core::Network::HTTP::Request::reinit(void) {
  this->Core::Network::HTTP::AMessage::reinit();
  this->method = "";
  this->url = "";
  this->success = nullptr;
  this->error = nullptr;
  this->clean = nullptr;
  this->file.isFile = false;
  this->file.filepath = "";
  this->asynchronous.isAsynchronous = true;
  this->asynchronous.isValid = false;
  this->cleanResponse();
}

void  Core::Network::HTTP::Request::cleanResponse(void) {
  if (this->asynchronous.response != nullptr) {
    Core::Network::HTTP::Response::returnToPool(this->asynchronous.response);
  }
  this->asynchronous.response = nullptr;
}

bool  Core::Network::HTTP::Request::wait(void) {
  SCOPELOCK(&(this->asynchronous.lock));
  this->asynchronous.lock.wait([this] () -> bool { return !this->asynchronous.isValid || this->asynchronous.response; });

  return this->asynchronous.isValid || this->asynchronous.response;
}

void Core::Network::HTTP::Request::wake(Core::Network::HTTP::Response *response) {
  SCOPELOCK(&(this->asynchronous.lock));
  this->asynchronous.response = response;
  this->asynchronous.isValid = (response != nullptr);
  this->asynchronous.lock.notify();
}
