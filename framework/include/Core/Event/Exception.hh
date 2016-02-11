#ifndef   __CORE_EVENT_EXCEPTION_HH__
#define   __CORE_EVENT_EXCEPTION_HH__

#include  "Core/Exception.hh"

namespace fwk {
  /**
   *  \class EventNotRegisteredException Core/Event/Exception.hh
   *  \brief Event thrown when trying to subscribe to an unregistered event.
   */
  class EventNotRegisteredException :public CoreException {
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

#endif    /* __CORE_EVENT_EXCEPTION_HH__ */
