#ifndef    __CORE_NETWORK_MANAGER_HH__
#define    __CORE_NETWORK_MANAGER_HH__

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/TCP/Manager.hh"
// #include  "Core/Network/UDP/Manager.hh"

namespace    Core {
  namespace  Network {
    class    Manager :public Singleton<Core::Network::Manager>, public Threading::Lock, public AEndable {
      friend class Singleton<Core::Network::Manager>;
    public:
      Threading::NotifiableThread _input;
      Threading::NotifiableThread _output;

      Core::Network::TCP::Manager  _tcp;
      // Core::Network::UDP::Manager  _udp;

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
      void init(void); // <- signal(SIGPIPE, SIG_IGN), and create threads

    private:
      void inputRoutine(void);
      void outputRoutine(void);

    public:
      Core::Network::TCP::Manager& getTCP(void);
      // Core::Network::UDP::Manager& getUDP(void);
    };
  }
}

#endif    /* __CORE_NETWORK_MANAGER_HH__ */
