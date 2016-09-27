#ifndef    __CORE_EXCEPTION_HH__
#define    __CORE_EXCEPTION_HH__

#include  "Library/Exception.hh"

namespace fwk {
    /**
     *  \class CoreException Core/Exception.hh
     *  \brief Superclass of every exception thrown in the Core.
     */
    class CoreException :public fwk::Exception {
    public:
        /**
         *  \brief Constructor of CoreException.
         *  \param message the error message.
         */
        CoreException(const std::string& message) noexcept;

        /**
         *  \brief Destructor of CoreException.
         */
        virtual ~CoreException(void) noexcept;
    };
}

#endif    /* __CORE_EXCEPTION_HH__ */
