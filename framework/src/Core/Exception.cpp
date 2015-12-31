#include  "Core/Exception.hh"

Core::Exception::Exception(const std::string& msg) noexcept:
  ::Exception(msg)
{}

Core::Exception::~Exception() {}
