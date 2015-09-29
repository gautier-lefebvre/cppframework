#include	"Core/Network/Exception.hh"

Core::Network::Exception::Exception(const std::string& msg) noexcept:
	Core::Exception(msg)
{}

Core::Network::Exception::~Exception() {}