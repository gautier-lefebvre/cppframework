#ifndef		__CORE_EVENT_MANAGER_HH__
#define		__CORE_EVENT_MANAGER_HH__

#include	<functional>
#include	<unordered_map>

#include	"Library/DesignPattern/Singleton.hpp"
#include	"Library/Threading/Lock.hpp"
#include	"Core/Event/EventInfo.hh"

namespace		Core {
	namespace	Event {
		class	Manager :public Singleton<Core::Event::Manager>, public Threading::Lock {
			friend class Singleton<Core::Event::Manager>;
		private:
			std::unordered_map<const Core::Event::EventBase*, Core::Event::EventInfo>	_events;

		private:
			Manager(const Manager&) = delete;
			Manager(const Manager&&) = delete;
			Manager& operator=(const Manager&) = delete;

		public:
			Manager(void);
			virtual ~Manager(void);

		public:
			void	register(const Core::Event::EventBase*, const std::function<void (Core::Event::IEventArgs*)>&);
			void	unregister(const Core::Event::EventBase*);

			void	subscribe(const Core::Event::EventBase*, const std::function<void (const Core::Event::IEventArgs*)>&, const void *);
			void	unsubscribe(const Core::Event::EventBase*, const void *);

		public:
			const Core::Event::EventInfo&	getInfo(const Core::Event::EventBase*) const;
		};
	}
}

#endif		/* __CORE_EVENT_MANAGER_HH__ */
