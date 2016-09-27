#include  <cstring>

#include  "Library/Encryption/XOR.hh"

using namespace fwk;

void  Encryption::XOR::xorify(void *data, size_t dataSize, const void* key, size_t keySize) {
  if (key != nullptr && data != nullptr && dataSize > 0 && keySize > 0) {
    uint8_t* dataPtr = reinterpret_cast<uint8_t*>(data);
    const uint8_t* keyPtr = reinterpret_cast<const uint8_t*>(key);
    for (size_t i = 0 ; i < dataSize ; ++i) {
      dataPtr[i] ^= keyPtr[i % keySize];
    }
  }
}
