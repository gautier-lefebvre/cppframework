#include  "Core/Network/Http/HttpResponse.hh"

using namespace fwk;

HttpResponse::HttpResponse(void):
  AHttpMessage(),
  status(0),
  reason("")
{}

HttpResponse::~HttpResponse(void) {}

void  HttpResponse::reinit(void) {
  this->AHttpMessage::reinit();
  this->status = 0;
  this->reason = "";
}
