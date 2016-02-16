#include  "Core/Network/Http/HttpRequest.hh"

using namespace fwk;

HttpRequest::HttpRequest(void):
  AHttpMessage(),
  APooled<HttpRequest>(),
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

HttpRequest::~HttpRequest(void) {}

void  HttpRequest::reinit(void) {
  this->AHttpMessage::reinit();
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

void  HttpRequest::cleanResponse(void) {
  if (this->asynchronous.response != nullptr) {
    HttpResponse::returnToPool(this->asynchronous.response);
  }
  this->asynchronous.response = nullptr;
}

bool  HttpRequest::wait(void) {
  SCOPELOCK(&(this->asynchronous.lock));
  this->asynchronous.lock.wait([this] () -> bool { return !this->asynchronous.isValid || this->asynchronous.response; });

  return this->asynchronous.isValid || this->asynchronous.response;
}

void HttpRequest::wake(HttpResponse *response) {
  SCOPELOCK(&(this->asynchronous.lock));
  this->asynchronous.response = response;
  this->asynchronous.isValid = (response != nullptr);
  this->asynchronous.lock.notify();
}
