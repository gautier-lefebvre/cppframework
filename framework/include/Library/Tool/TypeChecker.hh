#ifndef    __LIBRARY_TOOL_TYPECHECKER_HH__
#define    __LIBRARY_TOOL_TYPECHECKER_HH__

#include  <string>

namespace fwk {
  /**
   *  \class TypeChecker Library/Tool/TypeChecker.hh
   *  \brief a class with static methods to check if a string represents specific types.
   */
  class TypeChecker {
  public:
    /**
     *  \brief checks if a string represents a signed integer.
     *  \param str the string.
     *  \return true if the string represents a signed integer.
     */
    static bool  isInteger(const std::string& str);

    /**
     *  \brief checks if a string represents an unsigned integer.
     *  \param str the string.
     *  \return true if the string represents an unsigned integer.
     */
    static bool  isUnsignedInteger(const std::string& str);

    /**
     *  \brief checks if a string represents an unsigned hexadecimal integer, without the "0x" prefix.
     *  \param str the string.
     *  \return true if the string represents an hexadecimal integer.
     */
    static bool  isHexadecimal(const std::string& str);

    /**
     *  \brief checks if a string represents an MD5 hash (32 characters hexadecimal value).
     *  \param str the string.
     *  \return true if the string represents an MD5 hash.
     */
    static bool  isMD5Hash(const std::string& str);

  private:
    /**
     *  \brief Checks if every character of str are part of the characters string.
     *  \param str the string.
     *  \param characters the characters.
     *  \return true if every character of str are part of the characters string.
     */
    static bool  charactersIn(const std::string& str, const std::string& characters);
  };
}

#endif    /* __LIBRARY_TOOL_TYPECHECKER_HH__ */