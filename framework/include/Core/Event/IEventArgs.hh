#ifndef   __CORE_EVENT_IEVENTARGS_HH__
#define   __CORE_EVENT_IEVENTARGS_HH__

namespace fwk {
  /**
   *  \class IEventArgs Core/Event/IEventArgs.hh
   *  \brief Interface of any wrapper of Event arguments.
   *  Event arguments must be pooled.
   */
  class IEventArgs {
  public:
    /**
     *  \brief Destructor of IEventArgs.
     */
    virtual ~IEventArgs(void) {}

  public:
    /**
     *  \brief The child class must use this method to return itself to its pool after the event has been executed by each callback.
     *  This is only useful if the child class is pooled.
     */
    virtual void  cleanup(void) = 0;
  };
}

#endif    /* __CORE_EVENT_IEVENTARGS_HH__ */
