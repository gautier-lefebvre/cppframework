#ifndef   __CORE_EVENT_IEVENTARGS_HH__
#define   __CORE_EVENT_IEVENTARGS_HH__

#include  "Library/Factory/AFactored.hh"

namespace fwk {
  namespace Core {
    namespace Event {
      /**
       *  \class IEventArgs Core/Event/IEventArgs.hh
       *  \brief Interface of any wrapper of Event arguments.
       *  Event arguments must be pooled.
       */
      class IEventArgs :public Factory::AFactored {
      public:
        /**
         *  \brief Destructor of IEventArgs.
         */
        virtual ~IEventArgs(void) {}

      public:
        /**
         *  \brief reinit method the child must implement.
         *  Inherited from AFactored.
         */
        virtual void  reinit(void) = 0;

      public:
        /**
         *  \brief The child class must use this method to return any pooled object to factory after the event has been executed by each callback.
         */
        virtual void  cleanup(void) = 0;
      };
    }
  }
}

#endif    /* __CORE_EVENT_IEVENTARGS_HH__ */
