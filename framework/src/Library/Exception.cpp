#include  "Library/Exception.hh"

using namespace fwk;

Exception::Exception(const std::string& what) noexcept:
  std::exception(),
  _what(what)
{}

Exception::Exception(const std::exception& e) noexcept:
  std::exception(),
  _what(e.what())
{}

Exception& Exception::operator=(const std::exception& e) noexcept {
  if (this != &e) {
    this->_what = e.what();
  }
  return *this;
}

Exception::~Exception(void) {}

const char*  Exception::what(void) const noexcept {
  return this->_what.c_str();
}