#include "Library/Factory/Exception.hh"

using namespace fwk;

Factory::PoolNotInitializedException::PoolNotInitializedException(const std::string& msg) noexcept:
  ::Exception(msg)
{}

Factory::PoolNotInitializedException::~PoolNotInitializedException() {}

Factory::PoolInvalidArgumentsException::PoolInvalidArgumentsException(const std::string& msg) noexcept:
  ::Exception(msg)
{}

Factory::PoolInvalidArgumentsException::~PoolInvalidArgumentsException() {}

Factory::PoolMemoryExhaustedException::PoolMemoryExhaustedException(const std::string& msg) noexcept:
  ::Exception(msg)
{}

Factory::PoolMemoryExhaustedException::~PoolMemoryExhaustedException() {}
