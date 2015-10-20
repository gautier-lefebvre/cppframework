#ifndef    __CORE_EXCEPTION_HH__
#define    __CORE_EXCEPTION_HH__

#include  "Library/Exception.hh"

namespace  Core {
  class  Exception :public ::Exception {
  public:
    Exception(const std::string&) noexcept;
    virtual ~Exception(void) noexcept;
  };
}

#endif    /* __CORE_EXCEPTION_HH__ */
