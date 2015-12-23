#ifndef    __CORE_EXCEPTION_HH__
#define    __CORE_EXCEPTION_HH__

#include  "Library/Exception.hh"

namespace  Core {
  /**
   *  \class Exception Core/Exception.hh
   *  \brief Superclass of every exception thrown in the Core.
   */
  class  Exception :public ::Exception {
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

#endif    /* __CORE_EXCEPTION_HH__ */
