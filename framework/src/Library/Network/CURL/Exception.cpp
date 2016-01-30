#include  "Library/Network/CURL/Exception.hh"

curlxx::Exception::Exception(const std::string& msg) noexcept:
  ::Exception(msg)
{}

curlxx::Exception::~Exception() {}
