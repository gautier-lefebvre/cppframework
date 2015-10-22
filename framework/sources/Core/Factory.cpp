#include  "Core/Factory.hh"
#include  "Library/Factory/Pool.hpp"

void Core::Factory::init(void) {
  // init every pool (+ bytearray)
  ByteArray::initPool("ByteArray");
}

void Core::Factory::end(void) {
  // destroy every pool (+ byterray)
  ByteArray::destroyPool();
}