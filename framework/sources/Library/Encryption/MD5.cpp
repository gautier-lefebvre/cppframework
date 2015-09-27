#include	<openssl/md5.h>
#include	<iomanip>
#include	<sstream>
#include	<cstring>

#include	"Library/Encryption/MD5.hh"

const std::string Encryption::MD5::hash(const void* ptr, size_t length) {
	const uint8_t* target = reinterpret_cast<const uint8_t*>(ptr);
	uint8_t result[MD5_DIGEST_LENGTH];
	std::ostringstream oss;

	memset(result, 0, MD5_DIGEST_LENGTH);
	::MD5(target, length, result);
	for (size_t i = 0 ; i < MD5_DIGEST_LENGTH ; ++i) {
		oss << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint32_t>(result[i]);
	}
	return oss.str();
}