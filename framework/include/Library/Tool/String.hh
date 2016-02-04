#ifndef    __LIBRARY_TOOL_STRING_HH__
#define    __LIBRARY_TOOL_STRING_HH__

#include  <string>

namespace fwk {
  /**
   *  \brief Removes all white spaces at the beginning and the end of the string.
   *  \param str a reference on the string to trim.
   */
  void  trim(std::string& str);

  /**
   *  \brief Removes all white spaces at the beginning of the string.
   *  \param str a reference on the string to trim.
   */
  void  ltrim(std::string& str);

  /**
   *  \brief Removes all white spaces at the end of the string.
   *  \param str a reference on the string to trim.
   */
  void  rtrim(std::string& str);
}

#endif    /* __LIBRARY_TOOL_STRING_HH__ */