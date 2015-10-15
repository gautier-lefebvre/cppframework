#ifndef		__CORE_EVENT_EVENTBASE_HH__
#define		__CORE_EVENT_EVENTBASE_HH__

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

		class	EventBase {
		private:
			Source	_source;

		public:
			EventBase(Source);
			virtual ~EventBase();

		public:
			Source	getSource() const;
		};
	}
}

#endif		/* __CORE_EVENT_EVENTBASE_HH__ */
