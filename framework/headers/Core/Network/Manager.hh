#ifndef    __CORE_NETWORK_MANAGER_HH__
#define    __CORE_NETWORK_MANAGER_HH__

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/AEndable.hh"
#include  "Library/Property/Initializable.hpp"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/TCP/Manager.hh"
#include  "Core/Network/UDP/Manager.hh"

namespace    Core {
  /**
   *  \namespace Network
   *  \brief All networking classes (HTTP/TCP/UDP) and managers
   */
  namespace  Network {
    class    Manager :public Singleton<Core::Network::Manager>, public Threading::Lockable, public AEndable, public Initializable {
      friend class Singleton<Core::Network::Manager>;
    public:
      Threading::NotifiableThread _input;
      Threading::NotifiableThread _output;

      Core::Network::TCP::Manager  _tcp;
      Core::Network::UDP::Manager  _udp;

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
      void init(void);
      void endTCP(void);
      void endUDP(void);

    private:
      void inputRoutine(void);
      void outputRoutine(void);

    public:
      Core::Network::TCP::Manager& getTCP(void);
      Core::Network::UDP::Manager& getUDP(void);
    };
  }
}

#endif    /* __CORE_NETWORK_MANAGER_HH__ */
