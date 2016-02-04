#include  <algorithm>

#include  "Library/Tool/String.hh"

using namespace fwk;

void  fwk::ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void  fwk::rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

void  fwk::trim(std::string& s) {
  fwk::ltrim(s);
  fwk::rtrim(s);
}
