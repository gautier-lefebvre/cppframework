#ifndef   __CORE_EVENT_EXCEPTION_HH__
#define   __CORE_EVENT_EXCEPTION_HH__

#include  "Core/Exception.hh"

namespace Core {
  namespace Event {
    /**
     *  \class EventNotRegisteredException Core/Event/Manager.hh
     *  \brief Superclass of every exception thrown in the Core.
     */
    class  EventNotRegisteredException :public Core::Exception {
    public:
      /**
       *  \brief Constructor of EventNotRegisteredException.
       *  \param message the error message.
       */
      EventNotRegisteredException(const std::string& message = "This event was not registered") noexcept;

      /**
       *  \brief Destructor of EventNotRegisteredException.
       */
      virtual ~EventNotRegisteredException(void) noexcept;
    };
  }
}

#endif    /* __CORE_EVENT_EXCEPTION_HH__ */
