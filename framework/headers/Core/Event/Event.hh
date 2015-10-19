#ifndef		__CORE_EVENT_EVENT_HH__
#define		__CORE_EVENT_EVENT_HH__

#include	"Library/Factory/AFactored.hh"

namespace		Core {
	namespace	Event {
		enum class Source {
			CORE,
			APP
		};

		class	Event :public Factory::AFactored {
		private:
			Source	_source;

		public:
			Event(Source);
			virtual ~Event(void);

		public:
			virtual void	reinit(void);

		public:
			Source	getSource(void) const;
		};
	}
}

#endif		/* __CORE_EVENT_EVENT_HH__ */
