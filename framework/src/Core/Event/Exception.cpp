#include "Core/Event/Exception.hh"

using namespace fwk;

EventNotRegisteredException::EventNotRegisteredException(const std::string&& msg) noexcept:
    CoreException(std::forward<const std::string&>(msg))
{}

EventNotRegisteredException::~EventNotRegisteredException() {}
