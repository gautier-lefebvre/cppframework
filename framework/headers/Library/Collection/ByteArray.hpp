#ifndef		__LIBRARY_COLLECTION_BYTEARRAY_HPP__
#define		__LIBRARY_COLLECTION_BYTEARRAY_HPP__

#include	<iostream>
#include	<cstring>

#include	"Library/Threading/Lock.hpp"
#include	"Library/Factory/AFactored.hh"
#include	"Library/Factory/Pool.hpp"

/**
 *	\class ByteArray Library/Collection/ByteArray.hpp
 *	\brief Raw data container.
 */
class		ByteArray :public Factory::AFactored, public Threading::Lock {
public:
	struct	Pool {
		static Factory::Pool<ByteArray>	*_pool;

		static void init();
		static void destroy();

		static const size_t	ORIGINAL_SIZE = 100;
		static const size_t	HYDRATE_SIZE = 100;
		static const size_t	MAX_BUFFER_SIZE = 1000;

		static ByteArray*	create();
		static ByteArray*	create(size_t);
		static void			remove(ByteArray*);

		/**
		 *	\class Guard Library/Collection/ByteArray.hpp
		 *	\brief a guard class which sends a ByteArray back to the pool in its destructor.
		 */
		struct	Guard {
			ByteArray*	bytearray;

			Guard(ByteArray*);
			~Guard();
		};
	};

protected:
	uint8_t	*_bytearray; /*!< Pointer to the start of the data. */
	size_t	_size; /*!< Size of the data in bytes. */
	size_t	_sizemax; /*!< Maximum size of the data in bytes. */

public:
	/**
	 *	\brief Constructor of ByteArray.
	 *	\param size the maximum size of the data in bytes.
	 */
	ByteArray(size_t size = 1);

	/**
	 *	\brief Copy Constructor of ByteArray.
	 *
	 *	This will copy the data (and not just a pointer on the data).
	 *
	 *	\param oth The ByteArray to copy.
	 */
	ByteArray(const ByteArray& oth);

	/**
	 *	\brief Assignment Constructor of ByteArray.
	 *
	 *	This copies the data (and not just a pointer on the data).
	 *	This overwrites the previous data.
	 *	The maximum size of the data will be the highest value between the maximum size of \a oth and the current maximum size.
	 *
	 *	\param oth The ByteArray to copy.
	 */
	ByteArray& operator=(const ByteArray& oth);

	/**
	 *	\brief Destructor of ByteArray.
	 *
	 *	This deletes the data.
	 */
	virtual ~ByteArray();

public:
	/**
	 *	\brief Reinitializes the ByteArray.
	 *
	 *	Sets the current size to 0 and sets the data to an array of 0.
	 */
	virtual void reinit();

public:
	/**
	 *	\brief returns the current size.
	 *	\return the current size in bytes.
	 */
	size_t	getSize() const;

	/**
	 *	\brief returns the available space.
	 *	\return the available space in bytes (maximum size - current size).
	 */
	size_t	availableSpace() const;

	/**
	 *	\brief returns the maximum size.
	 *	\return the maximum size in bytes.
	 */
	size_t	getSizeMax() const;

	/**
	 *	\brief Checks if the ByteArray is empty.
	 *	\return \a true if the ByteArray is empty (i.e if the size is 0).
	 */
	bool empty() const;

	/**
	 *	\brief Checks if the ByteArray is full.
	 *	\return \a true if the ByteArray is full (i.e if the available space is 0).
	 */
	bool full() const;

	/**
	 *	\brief Returns the value of a byte at a given index.
	 *	\exception std::out_of_range thrown if \a idx is higher than the current size.
	 *	\param idx the index in bytes.
	 *	\return The value of the byte at index \a idx.
	 */
	uint8_t	operator[](size_t idx) const;

public:
	/**
	 *	\brief Clears the start of the ByteArray.
	 *
	 *	If the number of bytes to clear is higher than the current size, the ByteArray will simply be cleared to the current size.
	 *	This moves the data, so any pointer on the start of the data will now point to the new start.
	 *
	 *	\param idx the number of bytes to clear.
	 */
	void	clearStart(size_t idx);

	/**
	 *	\brief Moves the end of the data.
	 *	\throw std::out_of_range thrown if the offset is too high.
	 *	\param off the number of bytes to move the pointer of.
	 */
	void	moveEnd(size_t off);

public:
	/**
	 *	\brief Returns a pointer to the beginning of the data.
	 *	\return a pointer to the beginning of the data.
	 */
	const uint8_t*	getBytes() const;

	/**
	 *	\brief Returns a pointer to the end of the data.
	 *	\return a pointer to the end of the data.
	 */
	uint8_t* atEnd();

	/**
	 *	\brief Returns a pointer to the beginning of the data.
	 *	\return a pointer to the beginning of the data.
	 */
	uint8_t* atStart();

	/**
	 *	\brief Returns a pointer to a certain index of the data.
	 *	\param idx the index.
	 *	\return a pointer to the data at the index \a idx.
	 */
	uint8_t* atIndex(size_t idx);

public:
	/**
	 *	\brief Resizes the ByteArray to a certain size.
	 *
	 *	Resizes the ByteArray if the new size is higher than the current maximum size, or if \a force is \a true. 
	 *
	 *	\param size the number of bytes of the new ByteArray.
	 *	\param force if \a true, the ByteArray will be resized even if the current maximum size is higher than \a size.
	 *	\param keep if \a true, the current data is kept, up to \a size number of bytes.
	 */
	void	resize(size_t size, bool force = false, bool keep = false);

	/**
	 *	\brief Seeks a sequence of bytes inside the ByteArray.
	 *	\param ptr the start of the sequence to seek.
	 *	\param size the number of bytes of the sequence to seek.
	 *	\param start the index at which to start the search.
	 *	\return the index of the start of the sequence in the ByteArray, or `std::string::npos` if not found.
	 */
	size_t	find(const void* ptr, size_t size, size_t start = 0) const;

	/**
	 *	\brief Copies data from the start of the ByteArray.
	 *	\param ptr a pointer to which the data must be copied.
	 *	\param size the number of bytes to copy. If higher than the current size, only the current size will be copied.
	 *	\param offset the index of the byte where the copy must start.
	 *	\param remove if true, the ByteArray will be cleared of the data copied to \a ptr.
	 *	\return the number of bytes copied.
	 */
	size_t	get(void* ptr, size_t size, size_t offset, bool remove = true);

	size_t	get(void* ptr, size_t size, size_t offset) const;

public:
	/**
	 *	\brief Adds data to the end of the ByteArray.
	 *	\throw std::out_of_range thrown if the ByteArray does not have enough space to add the new data.
	 *	\param ptr a pointer to the start of the data to be copied.
	 *	\param size the number of bytes to copy.
	 *	\param resize if \a true, the ByteArray will be resized to have enough space to add the new data (if it didn't before).
	 */
 	template<typename T>
	void	push(const T* ptr, size_t size, bool resize = false) {
		if (resize) {
			this->resize(this->_size + size, false, true);
		}
		if (size > this->availableSpace()) {
			throw std::out_of_range("ByteArray: full");
		} else {
			memcpy(this->_bytearray + this->_size, reinterpret_cast<const uint8_t*>(ptr), size);
			this->_size += size;
		}
	}

	/**
	 *	\brief Adds a value to the end of the ByteArray, in binary (a \a uint32_t would take 4 bytes inside the ByteArray).
	 *	\throw std::out_of_range thrown if the ByteArray does not have enough space to add the new data.
	 *	\param val a value to copy.
	 *	\param resize if \a true, the ByteArray will be resized to have enough space to add the value (if it didn't before).
	 */
	template<typename T>
	void	push(const T& val, bool resize = false) {
		this->push(&val, sizeof(T), resize);
	}

	/**
	 *	\brief Adds a value to the start of the ByteArray.
	 *
	 *	The current data is moved, so any pointer to the start of the ByteArray will now point to the start of the new data.
	 *
	 *	\param ptr a pointer to the start of the data to be copied.
	 *	\param size the number of bytes to copy.
	 *	\param resize if \a true, the ByteArray will be resized to have enough space to add the new data (if it didn't before).
	 */
	template<typename T>
	void	push_front(const T* ptr, size_t size, bool resize = false) {
		if (resize) {
			this->resize(this->_size + size, false, true);
		}
		if (size > this->availableSpace()) {
			throw std::out_of_range("ByteArray: full");
		} else {
			memmove(this->_bytearray + size, this->_bytearray, size);
			memcpy(this->_bytearray, reinterpret_cast<const uint8_t*>(ptr), size);
			this->_size += size;
		}
	}

	/**
	 *	\brief Adds a value to the start of the ByteArray, in binary (a \a uint32_t would take 4 bytes inside the ByteArray).
	 *
	 *	The current data is moved, so any pointer to the start of the ByteArray will now point to the start of the new data.
	 *
	 *	\throw std::out_of_range thrown if the ByteArray does not have enough space to add the new data.
	 *	\param val a value to copy.
	 *	\param resize if \a true, the ByteArray will be resized to have enough space to add the value (if it didn't before).
	 */
	template<typename T>
	void	push_front(const T& val, bool resize = false) {
		this->push_front(&val, sizeof(T), resize);
	}

	/**
	 *	\brief Adds a std::string to the end of the ByteArray.
	 *
	 *	The number of bytes copied is the result of `str.length()`.
	 *
	 *	\throw std::out_of_range thrown if the ByteArray does not have enough space to add the string.
	 *	\param str a string to copy.
	 *	\param resize if \a true, the ByteArray will be resized to have enough space to add the string (if it didn't before).
	 */
	void	pushStr(const std::string& str, bool resize = false);

	/**
	 *	\brief Adds a std::string to the start of the ByteArray.
	 *
	 *	The current data is moved, so any pointer to the start of the ByteArray will now point to the start of the new data.
	 *	The number of bytes copied is the result of `str.length()`.
	 *
	 *	\throw std::out_of_range thrown if the ByteArray does not have enough space to add the string.
	 *	\param str a string to copy.
	 *	\param resize if \a true, the ByteArray will be resized to have enough space to add the string (if it didn't before).
	 */
	void	push_frontStr(const std::string& str, bool resize = false);
};

class ByteArrayExtractor {
private:
	const ByteArray *	_bytearray;
	size_t				_offset;

public:
	ByteArrayExtractor(const ByteArray *, size_t = 0);
	ByteArrayExtractor(const ByteArrayExtractor&);
	ByteArrayExtractor& operator=(const ByteArrayExtractor&);
	~ByteArrayExtractor();

public:
	void	extractString(std::string&, size_t);

	template<typename T>
	void	extract(T& data) {
		size_t size = static_cast<size_t>(sizeof(T));
		if (this->_bytearray->getSize() - this->_offset < size) {
			throw std::out_of_range("ByteArrayExtractor::extract: There are not enough bytes left in ByteArray to extract successfully");
		} else {
			this->_bytearray->get(&data, size, this->_offset);
			this->_offset += size;
		}
	}
};

/**
 *	\brief Adds a representation of the ByteArray to a stream.
 *
 *	The Array2 is printed in raw data (it may not be readable).
 *
 *	\param os the output stream.
 *	\param bytearray the ByteArray.
 *	\return a reference on the \a os parameter.
 */
std::ostream& operator<<(std::ostream& os, const ByteArray& bytearray);

#endif		/* __LIBRARY_COLLECTION_BYTEARRAY_HPP__ */