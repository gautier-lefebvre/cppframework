#ifndef    __LIBRARY_ENCRYPTION_XOR_HH__
#define    __LIBRARY_ENCRYPTION_XOR_HH__

#include  <cstdint>

namespace fwk {
    /**
     *  \namespace fwk::Encryption
     *  \brief Encryption classes.
     */
    namespace Encryption {
        /**
         *  \class XOR Library/Encryption/XOR.hh
         *  \brief a class which implements a XOR encryption algorithm (a^b).
         */
        class XOR {
        public:
            /**
             *  \brief Encrypts data with the current encryption key.
             *  \param data a pointer to the start of the data to encrypt.
             *  \param dataSize size of the data to encrypt in bytes.
             *  \param key a pointer to the start of the key used to encrypt.
             *  \param keySize size of the key in bytes.
             */
            static void  xorify(void* data, size_t dataSize, const void* key, size_t keySize);
        };
    }
}

#endif    /* __LIBRARY_ENCRYPTION_XOR_HH__ */
