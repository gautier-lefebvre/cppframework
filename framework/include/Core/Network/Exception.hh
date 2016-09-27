#ifndef    __CORE_NETWORK_EXCEPTION_HH__
#define    __CORE_NETWORK_EXCEPTION_HH__

#include  "Core/Exception.hh"

namespace fwk {  
    /**
     *  \class NetworkException Core/Network/Exception.hh
     *  \brief Superclass of every exception thrown in the Network.
     */
    class NetworkException :public CoreException {
        public:
        /**
         *  \brief Constructor of NetworkException.
         *  \param message the error message.
         */
        NetworkException(const std::string& message) noexcept;

        /**
         *  \brief Destructor of NetworkException.
         */
        virtual ~NetworkException(void) noexcept;
    };
}

#endif    /* __CORE_NETWORK_EXCEPTION_HH__ */
