#include  "Library/Factory/APooled.hpp"

using namespace fwk;

AFactored::AFactored(void):
    _valid(false),
    _lastOutOfPoolTimePoint()
{}

AFactored::~AFactored(void) {}

void  AFactored::isValid(bool valid) { this->_valid = valid; }

bool  AFactored::isValid(void) const { return this->_valid; }

void  AFactored::updateLastOutOfPoolTimePoint(void) {
    this->_lastOutOfPoolTimePoint = std::chrono::steady_clock::now();
}

const std::chrono::steady_clock::time_point& AFactored::lastOutOfPoolTimePoint() const { return this->_lastOutOfPoolTimePoint; }
