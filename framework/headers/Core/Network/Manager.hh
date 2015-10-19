#ifndef		__CORE_NETWORK_MANAGER_HH__
#define		__CORE_NETWORK_MANAGER_HH__

#include	"Library/DesignPattern/Singleton.hpp"
#include	"Library/Property/AEndable.hh"
#include	"Library/Threading/Lock.hpp"
#include	"Library/Threading/Condition.hpp"
#include	"Core/Network/TCP/Manager.hh"
#include	"Core/Network/UDP/Manager.hh"

namespace		Core {
	namespace	Network {
		class	Manager :public Singleton<Core::Network::Manager>, public Threading::Lock, public AEndable {
			friend class Singleton<Core::Network::Manager>;
		private:
			struct NotifiableThread {
				std::thread* 			thread;
				Threading::Condition	condition;
			};

		public:
			Core::Network::TCP::Manager	_tcp;
			Core::Network::UDP::Manager	_udp;

			NotifiableThread _input;
			NotifiableThread _output;

		private:
			Manager(const Manager&) = delete;
			Manager(const Manager&&) = delete;
			Manager& operator=(const Manager&) = delete;

		private:
			Manager(void);
			virtual ~Manager(void);

		public:
			virtual void end(void);

		public:
			Core::Network::TCP::Manager& getTCP(void);
			Core::Network::UDP::Manager& getUDP(void);
		};
	}
}

#endif		/* __CORE_NETWORK_MANAGER_HH__ */
