#ifndef		__LIBRARY_DESIGNPATTERN_SINGLETON_HPP__
#define		__LIBRARY_DESIGNPATTERN_SINGLETON_HPP__

#include	<cstddef>

/**
 *	\class Singleton Library/DesignPattern/Singleton.hpp
 *	\brief Base class of all singleton classes.
 *
 *	Implements the singleton design pattern.
 */
template<class T>
class Singleton {
private:
	static T	*_instance; /*!< A pointer on the instance of the subclass. */

private:
	/**
	 *	\brief deleted copy constructor.
	 */
	Singleton(const Singleton&) = delete;

	/**
	 *	\brief Deleted move constructor of Singleton.
	 */
	Singleton(const Singleton&&) = delete;

	/**
	 *	\brief deleted assignment constructor.
	 */
	Singleton& operator=(const Singleton&) = delete;

protected:
	/**
	 *	\brief Constructor of Singleton.
	 */
	Singleton(void) {}

	/**
	 *	\brief Destructor of Singleton.
	 */
	virtual ~Singleton(void) {}

public:
	/**
	 *	\brief Returns a reference on a Singleton subclass instance. Creates it before returning if it does not exist.
	 *	\return a reference on the subclass instance.
	 */
	static T&	get(void) {
		if (_instance == nullptr) {
			_instance = new T();
		}
		return (*_instance);
	}

	/**
	 *	\brief Returns a pointer on a Singleton subclass instance. Creates it before returning if it does not exist.
	 *	\return a pointer on the subclass instance.
	 */
	static T*	asPtr(void) {
		return &(get());
	}

	/**
	 *	\brief Destroys the singleton sublass instance.
	 */
	static void	destroy(void) {
		if (_instance) {
			delete (_instance);
		}
		_instance = nullptr;
	}
};

template<class T>
T*	Singleton<T>::_instance = nullptr;

#endif		/* __LIBRARY_DESIGNPATTERN_SINGLETON_HPP__ */
