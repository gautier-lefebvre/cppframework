#ifndef    __LIBRARY_PROPERTY_AENDABLE_HH__
#define    __LIBRARY_PROPERTY_AENDABLE_HH__

namespace fwk {
  /**
   *  \class AEndable Library/Property/AEndable.hh
   *  \brief Abstract class from which all classes than can be stopped by SIGINT inherit.
   */
  class AEndable {
  protected:
    bool  _end; /*!< set to true when the child class must end. */

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
     *  \brief Tells the child class that it must stop.
     */
    virtual void  end(void) = 0;

  public:
    /**
     *  \brief Checks if the end method has been called.
     *  \return true if the end method has been called.
     */
    bool  mustEnd(void) const;

    /**
     *  \brief Changes the end states of the object.
     *  \param state the new state.
     */
    void  mustEnd(bool state);
  };
}

#endif    /* __LIBRARY_PROPERTY_AENDABLE_HH__ */
