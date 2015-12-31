#include "Library/Property/Initializable.hpp"

Initializable::Initializable(void):
  _initialized(false)
{}

Initializable::~Initializable(void) {}

bool Initializable::isInitialized(void) const {
  return this->_initialized;
}

void Initializable::isInitialized(bool state) {
  this->_initialized = state;
}
