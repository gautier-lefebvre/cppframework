#ifndef   __CORE_SYSTEM_HH__
#define   __CORE_SYSTEM_HH__

#include  <string>
#include  <initializer_list>

#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Library/Tool/Logger.hpp"

namespace Core {
  class   System :public Threading::Lockable, public AEndable {
  private:
    enum  Modules {
      HTTP_CLIENT,
      UDP,
      TCP,
      WORKERS,
      LOGGER
    };

  public:
    Threading::Condition    _endCondition;
    std::map<Modules, bool> _modulesInitialized;

  public:
    System(void);
    virtual ~System(void);

  public:
    virtual void end(void);

  public:
    virtual void initHTTP(const std::string& userAgent);
    virtual void endHTTP(void);

    virtual void initTCP(void);
    virtual void endTCP(void);

    virtual void initUDP(void);
    virtual void endUDP(void);

    virtual void initWorkerThreads(size_t nbWorkerThreads, bool enableDelayedTasks);
    virtual void endWorkerThreads(void);

    virtual void initLogger(Logger::Level);

    virtual void run(void);
    virtual void cleanup(void);
  };
}

#endif    /* __CORE_SYSTEM_HH__ */
