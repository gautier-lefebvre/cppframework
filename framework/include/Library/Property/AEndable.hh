#ifndef    __LIBRARY_PROPERTY_AENDABLE_HH__
#define    __LIBRARY_PROPERTY_AENDABLE_HH__

#include  "Library/Threading/Lockable.hpp"

namespace fwk {
    /**
     *  \class AEndable Library/Property/AEndable.hh
     *  \brief Abstract class from which all classes than can be stopped by SIGINT inherit.
     */
    class AEndable {
    protected:
        bool     _end; /*!< set to true when the child class must end. */
        Lockable _endLock;

    public:
        /**
         *  \brief Constructor of AEndable.
         *
         *  Sets the end state to false.
         */
        AEndable(void);

        /**
         *  \brief Destructor of AEndable.
         */
        virtual ~AEndable(void);

    public:
        /**
         *  \brief End procedure of the child class.
         */
        virtual void  onEnd(void) = 0;

    public:
        /**
         *  \brief Checks if the method was not already called and calls onEnd.
         */
        void  end(void);

        /**
         *  \brief Checks if the end method has been called.
         *  \return true if the end method has been called.
         */
        bool  isEnding(void) const;

        /**
         *  \brief Changes the end states of the object.
         *  \param state the new state.
         */
        void  isEnding(bool state);
    };
}

#endif    /* __LIBRARY_PROPERTY_AENDABLE_HH__ */
