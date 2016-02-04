#ifndef    __LIBRARY_NETWORK_CURL_EXCEPTION_HH__
#define    __LIBRARY_NETWORK_CURL_EXCEPTION_HH__

#include  "Library/Exception.hh"

namespace fwk {
  namespace curlxx {
    /**
     *  \class Exception Library/Network/CURL/Exception.hh
     *  \brief Superclass of every exception thrown in CURLxx.
     */
    class Exception :public fwk::Exception {
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

#endif    /* __LIBRARY_NETWORK_CURL_EXCEPTION_HH__ */
