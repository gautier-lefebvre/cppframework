#include  "Library/Network/CURL/Exception.hh"

using namespace fwk;

curlxx::Exception::Exception(const std::string& msg) noexcept:
  ::Exception(msg)
{}

curlxx::Exception::~Exception() {}
