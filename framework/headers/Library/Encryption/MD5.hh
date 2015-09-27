#ifndef		__LIBRARY_ENCRYPTION_MD5_HH__
#define		__LIBRARY_ENCRYPTION_MD5_HH__

#include	<string>

/**
 *	\namespace Encryption
 *	\brief namepace which contains classes to encrypt / decrypt / hash data.
 */
namespace	Encryption {
	/**
	 *	\class MD5 Lbrary/Encryption/MD5.hh
	 *	\brief implements the MD5 hashing method.
	 */
	class	MD5 {
	public:
		/**
		 *	\brief hashes the data given in parameter.
		 *	\param ptr a pointer on the start of the data to hash.
		 *	\param size the size of the data to hash in bytes.
		 *	\return a string containing the MD5 hash of the data given.
		 */
		static const std::string hash(const void* ptr, size_t size);
	};
}

#endif		/* __LIBRARY_ENCRYPTION_MD5_HH__ */