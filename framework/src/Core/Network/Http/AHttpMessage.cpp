#include  <strings.h>

#include  "Core/Network/Http/AHttpMessage.hh"

using namespace fwk;

AHttpMessage::AHttpMessage(void):
  version("HTTP/1.1"),
  headers(),
  body(nullptr)
{}

AHttpMessage::~AHttpMessage(void) {
  this->reinit();
}

void  AHttpMessage::reinit(void) {
  if (this->body != nullptr) {
    ByteArray::returnToPool(this->body);
  }
  this->body = nullptr;
}

void  AHttpMessage::init(void) {
  if (this->body == nullptr) {
    this->body = ByteArray::getFromPool();
  }
}

const std::string&  AHttpMessage::getHeader(const std::string& header) const {
  for (auto &it : this->headers) {
    if (!strcasecmp(it.first.c_str(), header.c_str())) {
      return it.second;
    }
  }
  throw std::out_of_range("HTTP Message does not have header " + header);
}

bool  AHttpMessage::hasHeader(const std::string& header) const {
  for (auto &it : this->headers) {
    if (!strcasecmp(it.first.c_str(), header.c_str())) {
      return true;
    }
  }
  return false;
}
