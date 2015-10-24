#ifndef    __LIBRARY_FACTORY_POOL_HPP__
#define    __LIBRARY_FACTORY_POOL_HPP__

#include  <queue>
#include  <utility>
#include  <string>

#include  "Library/Threading/Lock.hpp"
#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/Converter.hpp"
#include  "Library/Exception.hh"

namespace  Factory {
  /**
   *  \class Pool Library/Factory/Pool.hpp
   *  \brief a templated pool to store objects without having to dynamically create them.
   */
  template<class C>
  class  Pool :public Threading::Lock {
  private:
    std::queue<C*> _pool; /*!< the collection of objects. */
    size_t         _hydrate; /*!< the number of objects to create when the pool is empty. */
    std::string    _name; /*!< the stored objects' class name. */

  public:
    /**
     *  \brief Constructor of Pool.
     *  \throw Exception size and hydrate must be greater than 0, or the memory is exhausted.
     *  Will create a first batch of objects.
     *  \param size the number of objects to create now.
     *  \param hydrate the number of objects to create when the pool is empty.
     *  \param classname the stored objects' classname.
     */
    Pool(size_t size, size_t hydrate, const std::string &classname):
      Threading::Lock::Lock(),
      _pool(),
      _hydrate(hydrate),
      _name(classname) {
      if (!size) {
        throw Exception(classname + ": Pool original size must be greater than 0");
      } else if (!hydrate) {
        throw Exception(classname + ": Pool hydrate size must be greater than 0");
      } else {
        this->hydrate(size);
      }
    }

    /**
     *  \brief Destructor of Pool.
     *
     *  Deletes every object still inside the pool.
     */
    virtual ~Pool(void) {
      SCOPELOCK(this);
      while (!(this->_pool.empty())) {
        delete this->_pool.front();
        this->_pool.pop();
      }
    }

    /**
     *  \brief Pops an object from the pool.
     *  \throw Exception the pool is empty and the memory is exhausted.
     *  If the pool is empty, will create the number of objects asked at creation.
     *  Will set the object's validity to true and its last out of pool time point to current time.
     *  \return the object popped from the pool.
     */
    C* get(void) {
      SCOPELOCK(this);
      if (this->_pool.empty()) {
        this->hydrate(this->_hydrate);
      }
      C*  element = this->_pool.front();
      this->_pool.pop();
      element->isValid(true);
      element->updateLastOutOfPoolTimePoint();
      return element;
    }

    template<typename... Args>
    C*  get(Args&... args) {
      C* item = this->get();
      item->init(args...);
      return item;
    }

    /**
     *  \brief Puts an object back into the pool.
     *  Will reinit the object and set its validity to false.
     *  If the \a element is nullptr, does nothing.
     *  \param element the object to put back into the pool.
     */
    void  push(C* element) {
      if (element != nullptr && element->isValid()) {
        element->reinit();
        element->isValid(false);
        {
          SCOPELOCK(this);
          this->_pool.push(element);
        }
      }
    }

  private:
    /**
     *  \brief Creates the specified number of objects and puts them into the pool.
     *  \param size the number of objects to create.
     *  \throw Exception memory exhausted.
     */
    void  hydrate(size_t size) {
      SCOPELOCK(this);
      try {
        for (size_t i = 0 ; i < size ; ++i) {
          C* element = new C();
          this->push(element);
        }
        DEBUG(this->_name + ": adding " + StringOfSize(size) + " items into the pool");
      } catch (const std::bad_alloc&) {
        throw Exception(this->_name + ": Memory exhausted while hydrating pool");
      }
    }
  };

  /**
   *  \class BasicPool Library/Factory/Pool.hpp
   *  \brief A base class for all objects pools.
   *  Base classes are intended to be singleton, otherwise this class is useless and Pool could be used.
   */
  template<class C, size_t O, size_t H>
  class HasBasicPool {
  protected:
    static Factory::Pool<C>* _pool; /*!< the pool of objects. */
    static const size_t POOL_ORIGINAL_SIZE = O;
    static const size_t POOL_HYDRATE_SIZE  = H;

  public:
    /**
     *  \brief Initializes the pool if it was not already created.
     *  \param originalSize the number of objects to create now.
     *  \param hydrateSize the number of objects to create when the pool is empty.
     *  \param className the stored objects' class name.
     */
    static void  initPool(const std::string& className) {
      if (_pool == nullptr) {
        _pool = new Factory::Pool<C>(
            POOL_ORIGINAL_SIZE,
            POOL_HYDRATE_SIZE,
            className);
      }
    }

    /**
     *  \brief Deletes the pool and sets it to nullptr.
     */
    static void  destroyPool(void) {
      if (_pool != nullptr) {
        delete _pool;
      }
      _pool = nullptr;
    }

    /**
     *  \brief Takes an object from the pool.
     *  The object must have an `init` method with the same arguments.
     *  \return the object.
     */
    template<typename... Args>
    static C*  getFromPool(Args&... args) {
      return _pool->get(args...);
    }

    /**
     *  \brief Puts an object back into the pool.
     *  \param element the object to put back into the pool.
     */
    static void  returnToPool(C* element) {
      if (element != nullptr) {
        _pool->push(element);
      }
    }
  };
}

template<class C, size_t O, size_t H>
Factory::Pool<C>*  Factory::HasBasicPool<C, O, H>::_pool = nullptr;

#endif    /* __LIBRARY_FACTORY_POOL_HPP__ */
