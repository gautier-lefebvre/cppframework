#ifndef		__LIBRARY_CONVERTER_HPP__
#define		__LIBRARY_CONVERTER_HPP__

#include	<string>
#include	<sstream>

/**
 *	\class Converter Library/Tool/Converter.hpp
 *	\brief A class with static methods to convert any type to a string, or from a string, given that there is a `std::ostream& operator<<(std::ostream&, const T&)` and `std::ostream& operator>>(std::ostream&, T&)` overload somewhere.
 */
class		Converter {
public:
	/**
	 *	\brief Converts an object to a string representation.
	 *	The type must have a `std::ostream& operator<<(std::ostream&, const T&)` function somewhere.
	 *	\param val the value to convert.
	 *	\return the string.
	 */
	template<typename T>
	static std::string	StringOf(const T&val) {
		std::stringstream ss;

		ss << std::fixed << val;
		return (ss.str());
	}

	/**
	 *	\brief Use a string to create an object.
	 *	The type must have a `std::ostream& operator>>(std::ostream&, T&)` function somewhere.
	 *	\param val the string representation of an object.
	 *	\return the object.
	 */
	template<typename T>
	static T		StringTo(const std::string &val) {
		std::stringstream ss;
		T			ret;

		ss << val;
		ss >> ret;
		return (ret);
	}

	/**
	 *	\brief converts a type to another type, using their string representation in the middle.
	 *	\param val the value to convert.
	 *	\return the converted value.
	 */
	template<typename T, typename U>
	static U		convert(const T& val) {
		U			ret;
		std::stringstream	ss;

		ss << val;
		ss >> ret;
		return (ret);
	}
};

#define		StringOfInt32(x)		Converter::StringOf<uint32_t>(x)
#define		StringToInt32(x)		Converter::StringTo<uint32_t>(x)

#define		StringOfUInt32(x)		Converter::StringOf<uint32_t>(x)
#define		StringToUInt32(x)		Converter::StringTo<uint32_t>(x)

#define		StringOfUInt16(x)		Converter::StringOf<uint16_t>(x)
#define		StringToUInt16(x)		Converter::StringTo<uint16_t>(x)

#define		StringOfSize(x)			Converter::StringOf<size_t>(x)
#define		StringToSize(x)			Converter::StringTo<size_t>(x)

#define		STROFBOOL(x)			((x)  ? "true" : "false")
#define		STRTOBOOL(x)			(std::string(x) == "true" ? true : false);


#endif    /* __LIBRARY_CONVERTER_HPP__ */