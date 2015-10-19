#include	"Library/Threading/Condition.hpp"

Threading::Condition::Condition(void):
	Threading::Lock(),
	_condvar()
{}

Threading::Condition::~Condition(void) {}

void	Threading::Condition::wait(void) {
	this->_condvar.wait(*this);
}

void	Threading::Condition::notify(void) {
	this->_condvar.notify_one();
}

void	Threading::Condition::notify_all(void) {
	this->_condvar.notify_all();
}