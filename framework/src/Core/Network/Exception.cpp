#include  "Core/Network/Exception.hh"

using namespace fwk;

Core::Network::Exception::Exception(const std::string& msg) noexcept:
  Core::Exception(msg)
{}

Core::Network::Exception::~Exception(void) {}