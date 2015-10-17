#ifndef		__CORE_EVENT_IEVENTARGS_HH__
#define		__CORE_EVENT_IEVENTARGS_HH__

#include	"Library/Factory/AFactored.hh"

namespace		Core {
	namespace	Event {
		class	IEventArgs :public Factory::AFactored {
		public:
			virtual ~IEventArgs(void) {}

		public:
			virtual void reinit(void) = 0;
		};
	}
}

#endif		/* __CORE_EVENT_IEVENTARGS_HH__ */
