#include  "Library/Tool/TypeChecker.hh"

bool  TypeChecker::charactersIn(const std::string& str, const std::string& characters) {
  size_t  str_size  = str.size();
  size_t  char_size = characters.size();
  bool  found;
  for (size_t i = 0 ; i < str_size ; ++i) {
    found = false;
    for (size_t j = 0 ; j < char_size ; ++j) {
      if (str[i] == characters[j]) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

bool  TypeChecker::isInteger(const std::string& str) {
  if (str.empty()) {
    return false;
  }
  if (str[0] == '-') {
    if (str.size() == 1) {
      return false;
    }
    return TypeChecker::charactersIn(str.c_str() + 1, "0123456789");
  } else {
    return TypeChecker::charactersIn(str, "0123456789");
  }
}

bool  TypeChecker::isUnsignedInteger(const std::string& str) {
  if (str.empty()) {
    return false;
  } else {
    return TypeChecker::charactersIn(str, "0123456789");
  }
}

bool  TypeChecker::isHexadecimal(const std::string& str) {
  if (str.empty()) {
    return false;
  } else {
    return TypeChecker::charactersIn(str, "0123456789abcdefABCDEF");
  }
}

bool  TypeChecker::isMD5Hash(const std::string& str) {
  return (str.size() == 32 && TypeChecker::isHexadecimal(str));
}