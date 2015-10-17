#ifndef		__LIBRARY_COLLECTION_ARRAY_HPP__
#define		__LIBRARY_COLLECTION_ARRAY_HPP__

#include	<iostream>
#include	"Library/Tool/Converter.hpp"

/**
 *	\struct Array2 Library/Collection/Array.hpp
 *	\brief Templated structure which contains two values of a same type.
 */
template<typename T>
struct		Array2 {
public:
	T	x; /*!< first value of type T */
	T	y; /*!< second value of type T */

	/**
	 *	\brief Contructor of Array2.
	 *
	 *	Sets x and y to 0.
	 */
	Array2(void): x(0), y(0) {}

	/**
	 *	\brief Contructor of Array2.
	 *	\param x first value of the Array2.
	 *	\param y second value of the Array2.
	 */
	Array2(T x, T y): x(x), y(y) {}

	/**
	 *	\brief Copy Contructor of Array2.
	 *	\param oth the Array2 to copy. Must be of the same type.
	 */
	Array2(const Array2<T> &oth): x(oth.x), y(oth.y) {}

	/**
	 *	\brief Assignment Contructor of Array2.
	 *	\param oth the Array2 to copy. Must be of the same type.
	 */
	Array2& operator=(const Array2<T> &oth) {
		this->x = oth.x;
		this->y = oth.y;
		return (*this);
	}

	/**
	 *	\brief Adds the x and y values of the Array2 in parameter to the x and y values in the current object.
	 *	\param oth the Array2 whose values to add. Must be of the same type.
	 */
	Array2& operator+=(const Array2<T> &oth) {
		this->x = this->x + oth.x;
		this->y = this->y + oth.y;
		return (*this);
	}

	/**
	 *	\brief Subs the x and y values of the Array2 in parameter to the x and y values in the current object.
	 *	\param oth the Array2 whose values to sub. Must be of the same type.
	 */
	Array2& operator-=(const Array2<T> &oth) {
		this->x = this->x - oth.x;
		this->y = this->y - oth.y;
		return (*this);
	}

	/**
	 *	\brief Multiplies the x and y values of the Array2 in parameter with the x and y values in the current object.
	 *	\param oth the Array2 whose values to multiply. Must be of the same type.
	 */
	Array2& operator*=(const Array2<T> &oth) {
		this->x = this->x * oth.x;
		this->y = this->y * oth.y;
		return (*this);
	}

	/**
	 *	\brief Divides the x and y values of current object by the x and y values of the Array2 in parameter.
	 *	\param oth the Array2 whose values will divide the values of the current object. Must be of the same type.
	 */
	Array2& operator/=(const Array2<T> &oth) {
		this->x = this->x / oth.x;
		this->y = this->y / oth.y;
		return (*this);
	}

	/**
	 *	\brief Does a modulo between the x and y values of current object and the x and y values of the Array2 in parameter.
	 *	\param oth the Array2 whose values will be used to do the modulo. Must be of the same type.
	 */
	Array2& operator%=(const Array2<T> &oth) {
		this->x = this->x % oth.x;
		this->y = this->y % oth.y;
		return (*this);
	}

	/**
	 *	\brief Destructor of Array2.
	 */
	~Array2(void) {}
};

/**
 *	\struct Array3 Library/Collection/Array.hpp
 *	\brief Templated structure which contains three values of a same type.
 */
template<typename T>
struct		Array3 {
public:
	T		x; /*!< first value of type T */
	T		y; /*!< second value of type T */
	T		z; /*!< third value of type T */

	/**
	 *	\brief Contructor of Array3.
	 *
	 *	Sets x, y and z to 0.
	 */
	Array3(void): x(0), y(0), z(0) {}
	
	/**
	 *	\brief Contructor of Array3.
	 *	\param x first value of the Array3.
	 *	\param y second value of the Array3.
	 *	\param z third value of the Array3.
	 */
	Array3(T x, T y, T z): x(x), y(y), z(z) {}

	/**
	 *	\brief Copy Contructor of Array3.
	 *	\param oth the Array3 to copy. Must be of the same type.
	 */
	Array3(const Array3<T> &oth): x(oth.x), y(oth.y), z(oth.z) {}

	/**
	 *	\brief Assignment Contructor of Array3.
	 *	\param oth the Array3 to copy. Must be of the same type.
	 */
	Array3& operator=(const Array3<T> &oth) {
		this->x = oth.x;
		this->y = oth.y;
		this->z = oth.z;
		return (*this);
	}

	/**
	 *	\brief Adds the x y, and z values of the Array3 in parameter to the x, y and z values in the current object.
	 *	\param oth the Array3 whose values to add. Must be of the same type.
	 */
	Array3& operator+=(const Array3<T> &oth) {
		this->x = this->x + oth.x;
		this->y = this->y + oth.y;
		this->z = this->z + oth.z;
		return (*this);
	}

	/**
	 *	\brief Subs the x, y, and z values of the Array3 in parameter to the x, y, and z values in the current object.
	 *	\param oth the Array3 whose values to sub. Must be of the same type.
	 */
	Array3& operator-=(const Array3<T> &oth) {
		this->x = this->x - oth.x;
		this->y = this->y - oth.y;
		this->z = this->z - oth.z;
		return (*this);
	}

	/**
	 *	\brief Multiplies the x, y, and z values of the Array3 in parameter with the x, y, and z values in the current object.
	 *	\param oth the Array3 whose values to multiply. Must be of the same type.
	 */
	Array3& operator*=(const Array3<T> &oth) {
		this->x = this->x * oth.x;
		this->y = this->y * oth.y;
		this->z = this->z * oth.z;
		return (*this);
	}

	/**
	 *	\brief Divides the x, y, and z values of current object by the x, y, and z values of the Array3 in parameter.
	 *	\param oth the Array3 whose values will divide the values of the current object. Must be of the same type.
	 */
	Array3& operator/=(const Array3<T> &oth) {
		this->x = this->x / oth.x;
		this->y = this->y / oth.y;
		this->z = this->z / oth.z;
		return (*this);
	}

	/**
	 *	\brief Does a modulo between the x, y, and z values of current object and the x, y, and z values of the Array3 in parameter.
	 *	\param oth the Array3 whose values will be used to do the modulo. Must be of the same type.
	 */
 	Array3& operator%=(const Array3<T> &oth) {
		this->x = this->x % oth.x;
		this->y = this->y % oth.y;
		this->z = this->z % oth.z;
		return (*this);
	}

	/**
	 *	\brief Destructor of Array3.
	 */
	~Array3(void) {}
};

/**
 *	\brief Adds two Array2.
 *	\param a the first Array2.
 *	\param b the second Array2.
 *	\return an Array2 with `x = a.x + b.x` and `y = a.y + b.y`.
 */
template<typename T>
Array2<T>	operator+(const Array2<T> &a, const Array2<T> &b) {
	return (Array2<T>(a.x + b.x, a.y + b.y));
}

/**
 *	\brief Subs two Array2.
 *	\param a the first Array2.
 *	\param b the second Array2.
 *	\return an Array2 with `x = a.x - b.x` and `y = a.y - b.y`.
 */
template<typename T>
Array2<T>	operator-(const Array2<T> &a, const Array2<T> &b) {
	return (Array2<T>(a.x - b.x, a.y - b.y));
}

/**
 *	\brief Multiplies two Array2.
 *	\param a the first Array2.
 *	\param b the second Array2.
 *	\return an Array2 with `x = a.x * b.x` and `y = a.y * b.y`.
 */
template<typename T>
Array2<T>	operator*(const Array2<T> &a, const Array2<T> &b) {
	return (Array2<T>(a.x * b.x, a.y * b.y));
}

/**
 *	\brief Divides two Array2.
 *	\param a the first Array2.
 *	\param b the second Array2.
 *	\return an Array2 with `x = a.x / b.x` and `y = a.y / b.y`.
 */
template<typename T>
Array2<T>	operator/(const Array2<T> &a, const Array2<T> &b) {
	return (Array2<T>(a.x / b.x, a.y / b.y));
}

/**
 *	\brief Does a modulo between two Array2.
 *	\param a the first Array2.
 *	\param b the second Array2.
 *	\return an Array2 with `x = a.x % b.x` and `y = a.y % b.y`.
 */
template<typename T>
Array2<T>	operator%(const Array2<T> &a, const Array2<T> &b) {
	return (Array2<T>(a.x % b.x, a.y % b.y));
}

/**
 *	\brief Adds two Array3.
 *	\param a the first Array3.
 *	\param b the second Array3.
 *	\param c the third Array3.
 *	\return an Array3 with `x = a.x + b.x`, `y = a.y + b.y` and `z = a.z + b.z`.
 */
template<typename T>
Array3<T>	operator+(const Array3<T> &a, const Array3<T> &b) {
	return (Array3<T>(a.x + b.x, a.y + b.y, a.z + b.z));
}

/**
 *	\brief Subs two Array3.
 *	\param a the first Array3.
 *	\param b the second Array3.
 *	\param c the third Array3.
 *	\return an Array3 with `x = a.x - b.x`, `y = a.y - b.y` and `z = a.z - b.z`.
 */
template<typename T>
Array3<T>	operator-(const Array3<T> &a, const Array3<T> &b) {
	return (Array3<T>(a.x - b.x, a.y - b.y, a.z - b.z));
}

/**
 *	\brief Multiplies two Array3.
 *	\param a the first Array3.
 *	\param b the second Array3.
 *	\param c the third Array3.
 *	\return an Array3 with `x = a.x * b.x`, `y = a.y * b.y` and `z = a.z * b.z`.
 */
template<typename T>
Array3<T>	operator*(const Array3<T> &a, const Array3<T> &b) {
	return (Array3<T>(a.x * b.x, a.y * b.y, a.z * b.z));
}

/**
 *	\brief Divides two Array3.
 *	\param a the first Array3.
 *	\param b the second Array3.
 *	\param c the third Array3.
 *	\return an Array3 with `x = a.x / b.x`, `y = a.y / b.y` and `z = a.z / b.z`.
 */
template<typename T>
Array3<T>	operator/(const Array3<T> &a, const Array3<T> &b) {
	return (Array3<T>(a.x / b.x, a.y / b.y, a.z / b.z));
}

/**
 *	\brief Does a modulo between two Array3.
 *	\param a the first Array3.
 *	\param b the second Array3.
 *	\param c the third Array3.
 *	\return an Array3 with `x = a.x % b.x`, `y = a.y % b.y` and `z = a.z % b.z`.
 */
template<typename T>
Array3<T>	operator%(const Array3<T> &a, const Array3<T> &b) {
	return (Array3<T>(a.x % b.x, a.y % b.y, a.z % b.z));
}

/**
 *	\brief Adds a representation of the Array2 to a stream.
 *
 *	The Array2 is printed as `[x/y]`
 *
 *	\param os the output stream.
 *	\param arr the Array2.
 *	\return a reference on the \a os parameter.
 */
template<typename T>
std::ostream&	operator<<(std::ostream &os, const Array2<T> &arr) {
	os << "Array2: [" << arr.x << "/" << arr.y << "]";
	return (os);
}

/**
 *	\brief Adds a representation of the Array3 to a stream.
 *
 *	The Array3 is printed as `[x/y/z]`
 *
 *	\param os the output stream.
 *	\param arr the Array3.
 *	\return a reference on the \a os parameter.
 */
template<typename T>
std::ostream&	operator<<(std::ostream &os, const Array3<T> &arr) {
	os << "Array3: [" << arr.x << "/" << arr.y << "/" << arr.z << "]";
	return (os);
}

typedef		Array2<int32_t>		Array2i;
typedef		Array2<float>		Array2f;
typedef		Array2<double>		Array2d;
typedef		Array2<uint32_t>	Array2u;

typedef		Array3<int32_t>		Array3i;
typedef		Array3<float>		Array3f;
typedef		Array3<double>		Array3d;
typedef		Array3<uint32_t>	Array3u;

#endif  	/* __LIBRARY_COLLECTION_ARRAY_HPP__ */