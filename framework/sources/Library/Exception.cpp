#include	"Library/Exception.hh"

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

Exception::~Exception() {}

const char*	Exception::what(void) const noexcept {
	return this->_what.c_str();
}