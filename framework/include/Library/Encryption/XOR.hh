#ifndef    __LIBRARY_ENCRYPTION_XOR_HH__
#define    __LIBRARY_ENCRYPTION_XOR_HH__

#include  <cstdint>

#include  "Library/DesignPattern/Singleton.hpp"

namespace  Encryption {
  /**
   *  \class XOR Library/Encryption/XOR.hh
   *  \brief a class which implements a XOR encryption algorithm (a^b).
   */
  class  XOR :public Singleton<Encryption::XOR> {
    friend class Singleton<Encryption::XOR>;
  private:
    uint8_t    *_key; /*!< pointer to the start of the encryption key */
    size_t    _length; /*!< size of the encryption key in bytes */

  private:
    /**
     *  \brief Constructor of XOR.
     *
     *  The key is set to \a nullptr.
     */
    XOR(void);

    /**
     *  \brief Destructor of XOR.
     */
    virtual ~XOR(void);

  public:
    /**
     *  \brief Sets the encryption key.
     *  \param ptr pointer to the start of the encryption key.
     *  \param size size of the encryption key in bytes.
     */
    void  setKey(const void* ptr, size_t size);

    /**
     *  \brief Encrypts data with the current encryption key.
     *
     *  If the key is not set, does nothing.
     *
     *  \param addr a pointer to the start of the data to encrypt.
     *  \param size size of the data to encrypt in bytes.
     */
    void  xorify(void* addr, size_t size) const;

  private:
    /**
     *  \brief Resets the key to \a nullptr.
     */
    void  clear(void);
  };
}

#endif    /* __LIBRARY_ENCRYPTION_XOR_HH__ */