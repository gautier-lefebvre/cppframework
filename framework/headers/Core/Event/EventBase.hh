#ifndef		__CORE_EVENT_EVENTBASE_HH__
#define		__CORE_EVENT_EVENTBASE_HH__

#include	"Library/Factory/AFactored.hh"

namespace		Core {
	namespace	Event {
		enum class Source {
			CORE,
			APP
		};

		enum class Module {
			NETWORK_TCP,
			NETWORK_UDP,
			NETWORK_HTTP
		};

		class	EventBase :public Factory::AFactored {
		private:
			Source	_source;

		public:
			EventBase(Source);
			virtual ~EventBase(void);

		public:
			virtual void	reinit(void) = 0;

		public:
			Source	getSource(void) const;
		};
	}
}

#endif		/* __CORE_EVENT_EVENTBASE_HH__ */
