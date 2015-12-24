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
    /**
     *  \class Manager Core/Network/Manager.hh
     *  \brief Manager of TCP/UDP networks.
     */
    class    Manager :public Singleton<Core::Network::Manager>, public Threading::Lockable, public AEndable, public Initializable {
      friend class Singleton<Core::Network::Manager>;
    public:
      Threading::NotifiableThread _input; /*!< the input thread. */
      Threading::NotifiableThread _output; /*!< the output thread. */

      Core::Network::TCP::Manager  _tcp; /*!< the TCP network manager. */
      Core::Network::UDP::Manager  _udp; /*!<  the UDP network manager.*/

    private:
      /**
       *  \brief Deleted copy constructor of Manager.
       */
      Manager(const Manager&) = delete;

      /**
       *  \brief Deleted copy constructor of Manager.
       */
      Manager(const Manager&&) = delete;

      /**
       *  \brief Deleted assignment constructor of Manager.
       */
      Manager& operator=(const Manager&) = delete;

    private:
      /**
       *  \brief Constructor of Manager.
       */
      Manager(void);

      /**
       *  \brief Destructor of Manager.
       */
      virtual ~Manager(void);

    public:
      /**
       *  \brief Clears the I/O threads of TCP and UDP networks, closing any server and client.
       */
      virtual void end(void);

    public:
      /**
       *  \brief Creates the I/O threads the first time it is called.
       */
      void init(void);

      /**
       *  \brief Clears the TCP network, closing any server and client.
       */
      void endTCP(void);

      /**
       *  \brief Clears the UDP network, closing any server and client.
       */
      void endUDP(void);

    private:
      /**
       *  \brief Until the end method is called, reads on the UDP/TCP sockets.
       */
      void inputRoutine(void);

      /**
       *  \brief Until the end method is called, writes on the UDP/TCP sockets.
       */
      void outputRoutine(void);

    public:
      /**
       *  \return the TCP manager.
       */
      Core::Network::TCP::Manager& getTCP(void);

      /**
       *  \return the UDP manager.
       */
      Core::Network::UDP::Manager& getUDP(void);
    };
  }
}

#endif    /* __CORE_NETWORK_MANAGER_HH__ */
