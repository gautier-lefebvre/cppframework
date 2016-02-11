#include "Library/Factory/Exception.hh"

using namespace fwk;

PoolNotInitializedException::PoolNotInitializedException(const std::string& msg) noexcept:
  Exception(msg)
{}

PoolNotInitializedException::~PoolNotInitializedException() {}

PoolInvalidArgumentsException::PoolInvalidArgumentsException(const std::string& msg) noexcept:
  Exception(msg)
{}

PoolInvalidArgumentsException::~PoolInvalidArgumentsException() {}

PoolMemoryExhaustedException::PoolMemoryExhaustedException(const std::string& msg) noexcept:
  Exception(msg)
{}

PoolMemoryExhaustedException::~PoolMemoryExhaustedException() {}
