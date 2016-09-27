#include  "Core/Network/Http/HttpResponse.hh"

using namespace fwk;

HttpResponse::HttpResponse(void):
  AHttpMessage(),
  APooled<HttpResponse>(),
  status(0),
  reason(""),
  _isValid(false)
{}

HttpResponse::~HttpResponse(void) {}

void  HttpResponse::reinit(void) {
  this->AHttpMessage::reinit();
  this->status = 0;
  this->reason = "";
  this->_isValid = false;
}
