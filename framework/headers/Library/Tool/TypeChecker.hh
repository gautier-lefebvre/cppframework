#ifndef		__LIBRARY_TOOL_TYPECHECKER_HH__
#define		__LIBRARY_TOOL_TYPECHECKER_HH__

#include	<string>

class		TypeChecker {
public:
	static bool	isInteger(const std::string&);
	static bool	isUnsignedInteger(const std::string&);
	static bool	isHexadecimal(const std::string&);
	static bool	isMD5Hash(const std::string&);

private:
	static bool charactersIn(const std::string&, const std::string&);
};

#endif		/* __LIBRARY_TOOL_TYPECHECKER_HH__ */