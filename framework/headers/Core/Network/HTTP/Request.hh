#ifndef		__CORE_NETWORK_HTTP_REQUEST_HH__
#define		__CORE_NETWORK_HTTP_REQUEST_HH__

#include	<functional>

#include	"Core/Network/HTTP/AMessage.hh"
#include	"Core/Network/HTTP/Response.hh"

namespace			Core {
	namespace		Network {
		namespace	HTTP {
			struct	Request :public AMessage {
				std::string	method;
				std::string	url;
				bool		secure;
				std::function<void (const Core::Network::HTTP::Response *)> success;
				std::function<void (const Core::Network::HTTP::Response *)> error;
				std::function<void (void)> clean;

				Request(void);
				virtual ~Request(void);

				virtual void reinit(void);
			};
		}
	}
}

#endif		/* __CORE_NETWORK_HTTP_REQUEST_HH__ */
