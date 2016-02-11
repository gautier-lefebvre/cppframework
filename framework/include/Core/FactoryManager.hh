#ifndef   __CORE_FACTORYMANAGER_HH__
#define   __CORE_FACTORYMANAGER_HH__

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/Initializable.hpp"
#include  "Library/Threading/Lock.hpp"

namespace fwk {
  /**
   *  \class FactoryManager Core/FactoryManager.hh
   *  \brief Singleton class to initialize the Pools.
   */
  class FactoryManager :public Singleton<fwk::FactoryManager>, public Initializable, public Threading::Lockable {
    friend class Singleton<fwk::FactoryManager>;
  private:
    /**
     *  \brief Deleted copy constructor of FactoryManager.
     */
    FactoryManager(const FactoryManager&) = delete;

    /**
     *  \brief Deleted move constructor of FactoryManager.
     */
    FactoryManager(const FactoryManager&&) = delete;

    /**
     *  \brief Deleted assignment constructor of FactoryManager.
     */
    FactoryManager&  operator=(const FactoryManager&) = delete;

  private:
    /**
     *  \brief Constructor of FactoryManager.
     */
    FactoryManager(void);

    /**
     *  \brief Destructor of FactoryManager.
     */
    virtual ~FactoryManager(void);

  public:
    /**
     *  \brief Initializes every Pool of the Library and the Core.
     */
    void  init(void);

    /**
     *  \brief Deletes every Pool of the Library and the Core.
     */
    void  end(void);
  };
}

#endif    /* __CORE_FACTORYMANAGER_HH__ */
