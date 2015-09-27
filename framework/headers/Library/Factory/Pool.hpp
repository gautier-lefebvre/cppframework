#ifndef		__LIBRARY_FACTORY_POOL_HPP__
#define		__LIBRARY_FACTORY_POOL_HPP__

#include	<queue>
#include	<string>

#include	"Library/Threading/Lock.hpp"
#include	"Library/Tool/Logger.hpp"
#include	"Library/Tool/Converter.hpp"
#include	"Library/Exception.hh"

namespace	Factory {
	template<class C>
	class	Pool :public Threading::Lock {
	private:
		std::queue<C*>	_pool;
		size_t			_hydrate;
		std::string		_name;

	public:
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

		virtual ~Pool() {
			SCOPELOCK(this);
			while (!(this->_pool.empty())) {
				delete this->_pool.front();
				this->_pool.pop();
			}
		}

		C* get() {
			SCOPELOCK(this);
			if (this->_pool.empty()) {
				this->hydrate(this->_hydrate);
			}
			C*	element = this->_pool.front();
			this->_pool.pop();
			element->isValid(true);
			return element;
		}

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
		void	hydrate(size_t size) {
			SCOPELOCK(this);
			try {
				for (size_t i = 0 ; i < size ; ++i) {
					this->_pool.push(new C());
				}
				DEBUG(this->_name + ": adding " + StringOfSize(size) + " items into the pool");
			} catch (const std::bad_alloc&) {
				throw Exception(this->_name + ": Memory exhausted while hydrating pool");
			}
		}
	};
}

#endif		/* __LIBRARY_FACTORY_POOL_HPP__ */