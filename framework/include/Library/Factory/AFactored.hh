#ifndef    __LIBRARY_FACTORY_AFACTORED_HH__
#define    __LIBRARY_FACTORY_AFACTORED_HH__

#include  <chrono>

/**
 *  \namespace Factory
 *  \brief  classes to use pools.
 */
namespace  Factory {
  /**
   *  \class AFactored Library/Factory/AFactored.hh
   *  \brief base class from which any object that will be pooled must inherit.
   */
  class  AFactored {
  private:
    bool  _valid; /*!< true if the object is out of the pool. */
    std::chrono::steady_clock::time_point _lastOutOfPoolTimePoint; /*!< set at current time when the object if taken from the pool. */

  public:
    /**
     *  \brief Constructor of AFactored.
     *  Will set valid to false, infering that the object is created by the pool itself.
     */
    AFactored(void);

    /**
     *  \brief Destructor of AFactored.
     */
    virtual ~AFactored(void);

  public:
    /**
     *  \brief This method is called when the object is returned to the pool.
     *  Subclass object should return any pooled object it owns to its own pool.
     */
    virtual void reinit(void) = 0;

  public:
    /**
     *  \brief Updates the \a _valid attribute.
     *  \param valid true if the object is out of the pool, elsewise false.
     */
    void  isValid(bool valid);

    /**
     *  \brief Checks if the object is inside the pool.
     *  \return true if the object is out of the pool, elsewise false.
     */
    bool  isValid(void) const;

    /**
     *  \brief Sets the last out of pool time point to current time.
     *  This uses a steady clock.
     */
    void  updateLastOutOfPoolTimePoint(void);

    /**
     *  \brief Retrieves the last out of pool time point.
     *  \return the last time the object was taken out of the pool.
     *
     *  This can be useful to determine if an object was returned to the pool between some actions.
     */
    const std::chrono::steady_clock::time_point& lastOutOfPoolTimePoint(void) const;
  };
}

#endif    /* __LIBRARY_FACTORY_AFACTORED_HH__ */
