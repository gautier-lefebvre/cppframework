#ifndef   __CORE_FACTORY_HH__
#define   __CORE_FACTORY_HH__

#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Property/Initializable.hpp"
#include  "Library/Threading/Lock.hpp"

namespace fwk {
  namespace Core {
    /**
     *  \class Factory Core/Factory.hh
     *  \brief Singleton class to initialize the Pools.
     */
    class Factory :public Singleton<fwk::Core::Factory>, public Initializable, public Threading::Lockable {
      friend class Singleton<fwk::Core::Factory>;
    private:
      /**
       *  \brief Deleted copy constructor of Factory.
       */
      Factory(const Factory&) = delete;

      /**
       *  \brief Deleted move constructor of Factory.
       */
      Factory(const Factory&&) = delete;

      /**
       *  \brief Deleted assignment constructor of Factory.
       */
      Factory&  operator=(const Factory&) = delete;

    private:
      /**
       *  \brief Constructor of Factory.
       */
      Factory(void);

      /**
       *  \brief Destructor of Factory.
       */
      virtual ~Factory(void);

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
}

#endif    /* __CORE_FACTORY_HH__ */
