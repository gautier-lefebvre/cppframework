#include  <cstdlib>
#include  <ctime>

#include  "Library/Tool/Random.hh"

using namespace fwk;

void  Random::setSeed(unsigned int seed) {
  srand(seed);
}

void  Random::init(void) {
  Random::setSeed(time(nullptr));
}

int32_t Random::getInt(int32_t min, int32_t max) {
  return (rand() % (max - min) + min);
}