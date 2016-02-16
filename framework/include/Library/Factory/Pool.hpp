#ifndef    __LIBRARY_FACTORY_POOL_HPP__
#define    __LIBRARY_FACTORY_POOL_HPP__

#include  <queue>
#include  <utility>
#include  <string>

#include  "Library/Exception.hh"
#include  "Library/Factory/Exception.hh"
#include  "Library/Property/Initializable.hpp"
#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Library/Threading/Lockable.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/Converter.hpp"

namespace fwk {
  /**
   *  \class Pool Library/Factory/Pool.hpp
   *  \brief a templated pool to store objects without having to dynamically create them.
   */
  template<class C>
  class Pool :public Lockable, public Initializable {
  private:
    std::queue<C*> _pool; /*!< the collection of objects. */
    size_t         _hydrate; /*!< the number of objects to create when the pool is empty. */
    std::string    _name; /*!< the stored objects' class name. */

  public:
    /**
     *  \brief Constructor of Pool.
     */
    Pool(void):
      Lockable(),
      Initializable(),
      _pool(),
      _hydrate(0),
      _name()
    {}

    /**
     *  \brief Destructor of Pool.
     *
     *  Deletes every object still inside the pool.
     */
    virtual ~Pool(void) {
      this->clear();
    }

  public:
    /**
     *  \brief Initializes the pool.
     *  \throw PoolMemoryExhaustedException the memory is exhausted.
     *  \throw PoolInvalidArgumentsException hydrate must be greater than 0, or the memory is exhausted.
     *  Will create a first batch of objects.
     *  \param size the number of objects to create now.
     *  \param hydrate the number of objects to create when the pool is empty.
     *  \param classname the stored objects' classname.
     */
    void init(size_t size, size_t hydrate, const std::string& classname) {
      SCOPELOCK(this);

      if (!this->isInitialized()) {
        this->_hydrate = hydrate;
        this->_name = classname;

        if (!hydrate) {
          throw PoolInvalidArgumentsException(fmt::format("{0}: Pool hydrate size must be greater than 0", classname));
        } else {
          this->hydrate(size);
        }

        this->isInitialized(true);
      }
    }

    /**
     *  \brief Pops an object from the pool.
     *  \throw PoolMemoryExhaustedException the pool is empty and the memory is exhausted.
     *  \throw PoolNotInitializedException the pool is empty and was never initialized.
     *  If the pool is empty, will create the number of objects asked at initialization.
     *  Will set the object's validity to true and its last out of pool time point to current time.
     *  \return the object popped from the pool.
     */
    C*  get(void) {
      SCOPELOCK(this);

      // if the pool is empty, try to hydrate
      if (this->_pool.empty()) {
        // if the pool was never initialized, throw exception.
        if (this->isInitialized()) {
          this->hydrate(this->_hydrate);
        } else {
          throw PoolNotInitializedException();
        }
      }

      C*  element = this->_pool.front();
      this->_pool.pop();
      element->isValid(true);
      element->updateLastOutOfPoolTimePoint();

      return element;
    }

    /**
     *  \brief Does the same as the get method without arguments, and calls the init method of the object with the given arguments.
     *  \throw PoolMemoryExhaustedException the pool is empty and the memory is exhausted.
     *  \throw PoolNotInitializedException the pool is empty and was never initialized.
     *  \param args Arguments passed to the init method of the pooled object. If the pooled object has no init method, this won't compile.
     *  \return the object popped from the pool, initialized.
     */
    template<typename... Args>
    C*  get(Args&&... args) {
      C* item = this->get();
      item->init(std::forward<Args>(args)...);
      return item;
    }

    /**
     *  \brief Puts an object back into the pool.
     *  Will reinit the object and set its validity to false.
     *  If the \a element is nullptr, does nothing.
     *  \param element the object to put back into the pool.
     *  \param force true if the object is certified valid to be put into the pool. A null object still won't be added.
     */
    void  push(C* element, bool force = false) {
      if (element != nullptr && (force || element->isValid())) {
        element->reinit();
        element->isValid(false);
        {
          SCOPELOCK(this);
          this->_pool.push(element);
        }
      }
    }

    /**
     *  \brief Empties the Pool, by deleting every object in it.
     */
    void clear(void) {
      SCOPELOCK(this);
      while (!(this->_pool.empty())) {
        delete this->_pool.front();
        this->_pool.pop();
      }
    }

  private:
    /**
     *  \brief Creates the specified number of objects and puts them into the pool.
     *  \param size the number of objects to create.
     *  \throw PoolMemoryExhaustedException memory exhausted.
     */
    void  hydrate(size_t size) {
      SCOPELOCK(this);
      try {
        for (size_t i = 0 ; i < size ; ++i) {
          C* element = new C();
          this->push(element, true);
        }
      } catch (const std::bad_alloc&) {
        throw PoolMemoryExhaustedException(fmt::format("{0}: Memory exhausted while hydrating pool", this->_name));
      }
    }
  };
}

#endif    /* __LIBRARY_FACTORY_POOL_HPP__ */
