#ifndef		__LIBRARY_CONVERTER_HPP__
#define		__LIBRARY_CONVERTER_HPP__

#include	<string>
#include	<sstream>

class		Converter {
public:
	template<typename T>
	static std::string	StringOf(const T&val) {
		std::stringstream ss;

		ss << std::fixed << val;
		return (ss.str());
	}

	template<typename T>
	static T		StringTo(const std::string &val) {
		std::stringstream ss;
		T			ret;

		ss << val;
		ss >> ret;
		return (ret);
	}

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