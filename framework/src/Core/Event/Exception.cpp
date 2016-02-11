#include "Core/Event/Exception.hh"

using namespace fwk;

EventNotRegisteredException::EventNotRegisteredException(const std::string& msg) noexcept:
  CoreException(msg)
{}

EventNotRegisteredException::~EventNotRegisteredException() {}
