#include  "Core/Exception.hh"

using namespace fwk;

Core::Exception::Exception(const std::string& msg) noexcept:
  ::Exception(msg)
{}

Core::Exception::~Exception() {}
