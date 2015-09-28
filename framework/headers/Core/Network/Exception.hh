#ifndef		__CORE_NETWORK_EXCEPTION_HH__
#define		__CORE_NETWORK_EXCEPTION_HH__

#include	"Core/Exception.hh"

namespace		Core {
	namespace	Network {
		class	Exception :public Core::Exception {
		public:
			Exception(const std::string&) noexcept;
			virtual ~Exception(void) noexcept;
		};
	}
}

#endif		/* __CORE_NETWORK_EXCEPTION_HH__ */
