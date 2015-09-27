#include	"Library/Threading/Condition.hpp"

Threading::Condition::Condition():
	Threading::Lock(),
	_condvar()
{}

Threading::Condition::~Condition() {}

void	Threading::Condition::wait() {
	this->_condvar.wait(*this);
}

void	Threading::Condition::notify() {
	this->_condvar.notify_one();
}

void	Threading::Condition::notify_all() {
	this->_condvar.notify_all();
}