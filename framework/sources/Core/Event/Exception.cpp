#include "Core/Event/Exception.hh"

Core::Event::EventNotRegisteredException::EventNotRegisteredException(const std::string& msg) noexcept:
  Core::Exception(msg)
{}

Core::Event::EventNotRegisteredException::~EventNotRegisteredException() {}
