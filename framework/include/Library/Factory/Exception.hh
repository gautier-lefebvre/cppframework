#ifndef   __LIBRARY_FACTORY_EXCEPTION_HH__
#define   __LIBRARY_FACTORY_EXCEPTION_HH__

#include  "Library/Exception.hh"

namespace fwk {
  namespace Factory {
    /**
     *  \class PoolNotInitializedException Library/Factory/Exception.hh
     *  \brief Exception thrown when trying to get or return an object to a null Pool.
     */
    class PoolNotInitializedException :public fwk::Exception {
    public:
      /**
       *  \brief Constructor of PoolNotInitializedException.
       *  \param message the error message.
       */
      PoolNotInitializedException(const std::string& message = "This pool was not initialized") noexcept;

      /**
       *  \brief Destructor of PoolNotInitializedException.
       */
      virtual ~PoolNotInitializedException(void) noexcept;
    };

    /**
     *  \class PoolInvalidArgumentsException Library/Factory/Exception.hh
     *  \brief Exception thrown when invalid values are given to initialize a Pool.
     */
    class PoolInvalidArgumentsException :public fwk::Exception {
    public:
      /**
       *  \brief Constructor of PoolInvalidArgumentsException.
       *  \param message the error message.
       */
      PoolInvalidArgumentsException(const std::string& message) noexcept;

      /**
       *  \brief Destructor of PoolInvalidArgumentsException.
       */
      virtual ~PoolInvalidArgumentsException(void) noexcept;
    };

    /**
     *  \class PoolMemoryExhaustedException Library/Factory/Exception.hh
     *  \brief Exception thrown when catching a std::bad_alloc exception.
     */
    class PoolMemoryExhaustedException :public fwk::Exception {
    public:
      /**
       *  \brief Constructor of PoolMemoryExhaustedException.
       *  \param message the error message.
       */
      PoolMemoryExhaustedException(const std::string& message) noexcept;

      /**
       *  \brief Destructor of PoolMemoryExhaustedException.
       */
      virtual ~PoolMemoryExhaustedException(void) noexcept;
    };
  }
}

#endif    /* __LIBRARY_FACTORY_EXCEPTION_HH__ */
