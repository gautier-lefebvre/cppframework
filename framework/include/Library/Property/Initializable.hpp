#ifndef    __LIBRARY_PROPERTY_INITIALIZABLE_HPP__
#define    __LIBRARY_PROPERTY_INITIALIZABLE_HPP__

#include  <utility>

namespace fwk {
  /**
   *  \class Initializable Library/Property/Initializable.hpp
   *  \brief Abstract class from which all classes than can be init only once inherit.
   */
  class Initializable {
  protected:
    bool  _initialized; /*!< set to true when the child class has been initialized. */

  public:
    /**
     *  \brief Constructor of Initializable.
     *
     *  Sets the init state to false.
     */
    Initializable(void);

    /**
     *  \brief Destructor of Initializable.
     */
    virtual ~Initializable(void);

  public:
    /**
     *  \brief Checks if the child class has been initialized.
     *  \return true if the child class has been initialized.
     */
    bool  isInitialized(void) const;

  protected:
    /**
     *  \brief Changes the init state of the object.
     *  \param state the new state.
     */
    void  isInitialized(bool state);
  };

  /**
   *  \class TInitializable Library/Property/Initializable.hpp
   *  \brief Templated class to create an Initializable object more easily.
   */
  template<class C>
  class TInitializable :public C, public Initializable {
  public:
    /**
     *  \brief Constructor of TInitializable.
     *
     *  Whatever arguments are passed to the constructor are passed to the templated class.
     */
    template<typename... Args>
    TInitializable(Args&&... args):
      C(std::forward<Args>(args)...),
      Initializable()
    {}

    /**
     *  \brief Destructor of TInitializable.
     */
    virtual ~TInitializable(void) {}
  };
}

#endif    /* __LIBRARY_PROPERTY_INITIALIZABLE_HPP__ */
