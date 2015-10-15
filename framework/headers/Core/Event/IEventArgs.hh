#ifndef		__CORE_EVENT_IEVENTARGS_HH__
#define		__CORE_EVENT_IEVENTARGS_HH__

#include	"Library/Factory/AFactored.hh"

namespace		Core {
	namespace	Event {
		class	IEventArgs :public Factory::AFactored {
		public:
			virtual ~IEventArgs() {}

		public:
			virtual void reinit() = 0;
		};
	}
}

#endif		/* __CORE_EVENT_IEVENTARGS_HH__ */
