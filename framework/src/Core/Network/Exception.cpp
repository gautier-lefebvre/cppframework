#include  "Core/Network/Exception.hh"

using namespace fwk;

NetworkException::NetworkException(const std::string& msg) noexcept:
  CoreException(msg)
{}

NetworkException::~NetworkException(void) {}