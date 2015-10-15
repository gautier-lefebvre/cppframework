#ifndef		__CORE_NETWORK_HTTP_AMESSAGE_HH__
#define		__CORE_NETWORK_HTTP_AMESSAGE_HH__

#include	<map>
#include	<string>

#include	"Library/Collection/ByteArray.hpp"
#include	"Library/Factory/AFactored.hh"

namespace			Core {
	namespace		Network {
		namespace	HTTP {
			struct	AMessage :public Factory::AFactored {
			public:
				std::string							version;
				std::map<std::string, std::string>	headers;
				ByteArray							*body;

			public:
				AMessage();
				virtual ~AMessage();

			public:
				virtual void	reinit();

			public:
				void init();

			public:
				const std::string&	getHeader(const std::string& header) const;
				bool				hasHeader(const std::string& header) const;
			};
		}
	}	
}

#endif		/* __CORE_NETWORK_HTTP_AMESSAGE_HH__ */
