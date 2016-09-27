#ifndef    __LIBRARY_EXCEPTION_HH__
#define    __LIBRARY_EXCEPTION_HH__

#include  <string>
#include  <exception>

namespace fwk {
    /**
     *  \class Exception Library/Exception.hh
     *  \brief Base class of all the custom exceptions of the project.
     */
    class Exception :public std::exception {
    private:
        std::string  _what; /*!< a string describing the error. */

    public:
        /**
         *  \brief Constructor of Exception.
         *  \param what a string describing the error.
         */
        Exception(const std::string& what) noexcept;

        /**
         *  \brief Constructor of Exception.
         *  \param oth an exception to copy.
         */
        Exception(const std::exception& oth) noexcept;

        /**
         *  \brief Constructor of Exception.
         *  \param oth an exception to copy.
         */
        Exception& operator=(const std::exception& oth) noexcept;

        /**
         *  \brief Destructor of Exception.
         */
        virtual ~Exception(void);

        /**
         *  \brief Gets the error thrown.
         *  \return a string describing the error.
         */
        virtual const char*  what(void) const noexcept;
    };
}

#endif    /* __LIBRARY_EXCEPTION_HH__ */
