#ifndef		__LIBRARY_COLLECTION_ORDEREDLIST_HPP__
#define		__LIBRARY_COLLECTION_ORDEREDLIST_HPP__

#include	<list>
#include	<functional>

/**
 *	\class OrderedList Library/Collection/OrderedList.hpp
 *	\brief a std::list which keeps its values ordered.
 *
 *	Should not be used for gigantic lists (each push iterates through the list).
 *	To insert values, only the `push()` method should be used, otherwise the values will not be in order.
 */
template<typename T>
class	OrderedList :public std::list<T> {
private:
	std::function<bool (const T&, const T&)>	_compare; /*!< The function which will be used to compare the values inserted. */

public:
	/**
	 *	\brief Constructor of OrderedList.
	 *
	 *	The comparison method is a simple `v1 < v2`.
	 */
	OrderedList():
		std::list<T>(),
		_compare([] (const T& a, const T& b) -> bool { return a < b; })
	{}

	/**
	 *	\brief Constructor of OrderedList.
	 *	\param f the function which will be used to compare the values (should be used when dealing with a list of pointers).
	 */
	OrderedList(const std::function<bool (const T&, const T&)>& f):
		std::list<T>(),
		_compare(f)
	{}

	/**
	 *	\brief Destructor of OrderedList.
	 */
	virtual ~OrderedList() {}

	/**
	 *	\brief Inserts the value in the right place inside the list.
	 *
	 *	Must always be used to keep all values ordered.
	 *	\param val the value to insert.
	 */
	virtual void push(const T& val) {
		if (this->empty()) {
			this->push_back(val);
			return ;
		}
		for (auto it = this->begin() ; it != this->end() ; ++it) {
			if (this->_compare(val, *it)) {
				this->insert(it, val);
				return ;
			}
			else if (std::next(it,1) == this->end()) {
				this->push_back(val);
				return ;
			}
		}
	}
};

#endif		/* __LIBRARY_COLLECTION_ORDEREDLIST_HPP__ */