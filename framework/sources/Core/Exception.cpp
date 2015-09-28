#include	"Core/Exception.hh"

Core::Exception::Exception(const std::string& msg):
	::Exception(msg)
{}

Core::Exception::~Exception() {}