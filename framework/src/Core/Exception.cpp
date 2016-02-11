#include  "Core/Exception.hh"

using namespace fwk;

CoreException::CoreException(const std::string& msg) noexcept:
  Exception(msg)
{}

CoreException::~CoreException() {}
