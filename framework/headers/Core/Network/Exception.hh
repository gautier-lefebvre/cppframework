#ifndef    __CORE_NETWORK_EXCEPTION_HH__
#define    __CORE_NETWORK_EXCEPTION_HH__

#include  "Core/Exception.hh"

namespace    Core {
  namespace  Network {
    /**
     *  \class Exception Core/Network/Exception.hh
     *  \brief Superclass of every exception thrown in the Network.
     */
    class  Exception :public Core::Exception {
      public:
      /**
       *  \brief Constructor of Exception.
       *  \param message the error message.
       */
      Exception(const std::string& message) noexcept;

      /**
       *  \brief Destructor of Exception.
       */
      virtual ~Exception(void) noexcept;
    };
  }
}

#endif    /* __CORE_NETWORK_EXCEPTION_HH__ */
