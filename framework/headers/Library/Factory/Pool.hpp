#ifndef		__LIBRARY_FACTORY_POOL_HPP__
#define		__LIBRARY_FACTORY_POOL_HPP__

#include	<queue>
#include	<utility>
#include	<string>

#include	"Library/Threading/Lock.hpp"
#include	"Library/Tool/Logger.hpp"
#include	"Library/Tool/Converter.hpp"
#include	"Library/Exception.hh"

namespace	Factory {
	/**
	 *	\class Pool Library/Factory/Pool.hpp
	 *	\brief a templated pool to store objects without having to dynamically create them.
	 */
	template<class C>
	class	Pool :public Threading::Lock {
	private:
		std::queue<C*>	_pool; /*!< the collection of objects. */
		size_t			_hydrate; /*!< the number of objects to create when the pool is empty. */
		std::string		_name; /*!< the stored objects' class name. */

	public:
		/**
		 *	\brief Constructor of Pool.
		 *	\throw Exception size and hydrate must be greater than 0, or the memory is exhausted.
		 *	Will create a first batch of objects.
		 *	\param size the number of objects to create now.
		 *	\param hydrate the number of objects to create when the pool is empty.
		 *	\param classname the stored objects' classname.
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
		 *	\brief Destructor of Pool.
		 *
		 *	Deletes every object still inside the pool.
		 */
		virtual ~Pool(void) {
			SCOPELOCK(this);
			while (!(this->_pool.empty())) {
				delete this->_pool.front();
				this->_pool.pop();
			}
		}

		/**
		 *	\brief Pops an object from the pool.
		 *	\throw Exception the pool is empty and the memory is exhausted.
		 *	If the pool is empty, will create the number of objects asked at creation.
		 *	Will set the object's validity to true and its last out of pool time point to current time.
		 *	\return the object popped from the pool.
		 */
		C* get(void) {
			SCOPELOCK(this);
			if (this->_pool.empty()) {
				this->hydrate(this->_hydrate);
			}
			C*	element = this->_pool.front();
			this->_pool.pop();
			element->isValid(true);
			element->updateLastOutOfPoolTimePoint();
			return element;
		}

		/**
		 *	\brief Puts an object back into the pool.
		 *	Will reinit the object and set its validity to false.
		 *	If the \a element is nullptr, does nothing.
		 *	\param element the object to put back into the pool.
		 */
		void	push(C* element) {
			if (element != nullptr) {
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
		 *	\brief Creates the specified number of objects and puts them into the pool.
		 *	\param size the number of objects to create.
		 *	\throw Exception memory exhausted.
		 */
		void	hydrate(size_t size) {
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
	 *	\class BasicPool Library/Factory/Pool.hpp
	 *	\brief A base class for all objects pools.
	 *	Base classes are intended to be singleton, otherwise this class is useless and Pool could be used.
	 */
	template<class C>
	class BasicPool {
	protected:
		Factory::Pool<C>*	_pool; /*!< the pool of objects. */

	public:
		/**
		 *	\brief Constructor of BasicPool.
		 *	Sets the pool to nullptr.
		 */
		BasicPool(void): _pool(nullptr) {}

		/**
		 *	\brief Destructor of BasicPool.
		 *	Will delete the pool if it was set.
		 */
		virtual ~BasicPool(void) { this->destroyPool(); }

	public:
		/**
		 *	\brief Initializes the pool if it was not already created.
		 *	\param originalSize the number of objects to create now.
		 *	\param hydrateSize the number of objects to create when the pool is empty.
		 *	\param className the stored objects' class name.
		 */
		void	initPool(size_t originalSize, size_t hydrateSize, const std::string& className) {
			if (this->_pool == nullptr) {
				this->_pool = new Factory::Pool<C>(originalSize, hydrateSize, className);
			}
		}

		/**
		 *	\brief Deletes the pool and sets it to nullptr.
		 */
		void	destroyPool(void) {
			if (this->_pool != nullptr) {
				delete this->_pool;
			}
			this->_pool = nullptr;
		}

		/**
		 *	\brief Takes an object from the pool.
		 *	\return the object.
		 */
		C*	create(void) {
			return this->_pool->get();
		}

		/**
		 *	\brief Takes an object from the pool.
		 *	The object must have an `init` method with the same arguments.
		 *	\return the object.
		 */
		template<typename... Args>
		C*	create(Args&... args) {
			C* item = this->_pool->get();
			item->init(args...);
			return item;
		}

		/**
		 *	\brief Puts an object back into the pool.
		 *	\param element the object to put back into the pool.
		 */
		void	remove(C* element) {
			if (element != nullptr) {
				this->_pool->push(element);
			}
		}
	};
}

#endif		/* __LIBRARY_FACTORY_POOL_HPP__ */