#ifndef   __CORE_SYSTEM_HH__
#define   __CORE_SYSTEM_HH__

#include  <string>
#include  <map>
#include  <initializer_list>

#include  "Library/Property/AEndable.hh"
#include  "Library/Threading/Lock.hpp"
#include  "Library/Threading/Condition.hpp"
#include  "Library/Tool/Logger.hpp"

/**
 *  \namespace Core
 *  \brief Classes specific to the framework.
 */
namespace Core {
  /**
   *  \class System Core/System.hh
   *  \brief Main class of the framework (intended to be singleton).
   */
  class   System :public Threading::Lockable, public AEndable {
  private:
    /**
     *  \class Modules Core/System.hh
     *  \brief Enumeration of the initializable modules.
     */
    enum  Modules {
      HTTP,
      UDP,
      TCP,
      WORKERS,
    };

  public:
    Threading::Condition    _endCondition; /*!< the condition variable on which the main thread will wait until its end method is called. */
    std::map<Modules, bool> _modulesInitialized; /*!< state of initialization of each module. */

  public:
    /**
     *  \brief Constructor of System.
     *  Sets each module to uninitialized.
     */
    System(void);

    /**
     *  \brief Destructor of System.
     *  Ends each module and deletes every singleton.
     */
    virtual ~System(void);

  public:
    /**
     *  \brief wakes the main thread.
     */
    virtual void end(void);

  public:
    /**
     *  \brief Initializes the HTTP module.
     *  \param userAgent the user agent used for HTTP requests.
     */
    virtual void initHTTP(const std::string& userAgent);

    /**
     *  \brief Stops the HTTP module.
     *  Clears every request and wakes every thread waiting on synchronous requests.
     */
    virtual void endHTTP(void);

    /**
     *  \brief Initializes the I/O threads of TCP/UDP networking.
     */
    virtual void initTCP(void);

    /**
     *  \brief Clears the TCP module (close each server and client).
     *  If the UDP module is stopped, stops the I/O threads.
     */
    virtual void endTCP(void);

    /**
     *  \brief Initializes the I/O threads of TCP/UDP networking.
     */
    virtual void initUDP(void);

    /**
     *  \brief Clears the UDP module (close each server and client).
     *  If the TCP module is stopped, stops the I/O threads.
     */
    virtual void endUDP(void);

    /**
     *  \brief Initializes the workers with the given number of threads.
     *  \param nbWorkerThreads the number of worker threads used in the application.
     *  \param enableDelayedTasks activate or not an extra thread for delayed tasks.
     */
    virtual void initWorkerThreads(size_t nbWorkerThreads, bool enableDelayedTasks);

    /**
     *  \brief Stops the worker threads. Clears every pending tasks.
     */
    virtual void endWorkerThreads(void);

    /**
     *  \brief Waits until the end method is called.
     */
    virtual void run(void);

    /**
     *  \brief Ends every module and deletes the singletons.
     */
    virtual void cleanup(void);
  };
}

#endif    /* __CORE_SYSTEM_HH__ */
