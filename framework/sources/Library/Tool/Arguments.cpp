#include	<cstring>
#include	<stdexcept>
#include	<iostream>

#include	"Library/Tool/Arguments.hh"
#include	"Library/Tool/Converter.hpp"
#include	"Library/Tool/Macro.hh"
#include	"Library/Exception.hh"

Arguments::Arg::Arg(const std::string &flag):
	flag(flag)
{}

Arguments::Arg::Arg(const std::string& flag, const std::vector<std::string>& values):
	flag(flag),
	values(values)
{}

const std::string& Arguments::Arg::operator[](size_t idx) const {
	if (idx < this->values.size()) {
		return this->values[idx];
	} else {
		throw std::out_of_range("index " + StringOfSize(idx) + " out of range");
	}
}

Arguments::Expected::Expected(const std::string& flag, const std::string& type, const std::string& description, bool mandatory, const std::function<bool (const std::string&)>& tc, size_t nbmin, ssize_t nbmax):
	flag(flag),
	type(type),
	description(description),
	mandatory(mandatory),
	typechecker(tc),
	nbmin(nbmin),
	nbmax(nbmax)
{}

Arguments::Incompatible::Incompatible(const std::list<std::string>& flags, bool mandatory):
	flags(flags),
	oneMandatory(mandatory)
{}

Arguments::Arguments():
	_ac(0),
	_av(nullptr),
	_arguments(),
	_defaultValues(),
	_expected(),
	_incompatibleArguments()
{}

Arguments::~Arguments() {
	for (auto& it : this->_expected) {
		delete it;
	}
	for (auto& it : this->_arguments) {
		delete it;
	}
	for (auto& it : this->_defaultValues) {
		delete it;
	}
	for (auto& it : this->_incompatibleArguments) {
		delete it;
	}
}

void	Arguments::init(int ac, char **av) {
	this->_ac = ac;
	this->_av = av;
}

void	Arguments::defaultValue(const std::string &flag, const std::string &value) {
	Arguments::Arg* arg = this->find(this->_defaultValues, flag, true);
	arg->values.push_back(value);
}

void	Arguments::expected(Expected *ex) {
	this->_expected.push_back(ex);
}

void	Arguments::expected(const std::string &flag, const std::string &type, const std::string &description, bool mandatory, const std::function<bool (const std::string&)>& tc, size_t nbmin, ssize_t nbmax) {
	this->_expected.push_back(new Arguments::Expected(flag, type, description, mandatory, tc, nbmin, nbmax));
}

void	Arguments::incompatible(Arguments::Incompatible* incompatible) {
	for (auto& it: incompatible->flags) {
		if (!this->isExpected(it)) {
			throw ::Exception("Unexpected flag: " + it);
		}
	}
	this->_incompatibleArguments.push_back(incompatible);
}

void	Arguments::incompatible(const std::list<std::string>& flags, bool mandatory) {
	this->incompatible(new Arguments::Incompatible(flags, mandatory));
}

bool	Arguments::run() {
	int i = 1;
	while (i < this->_ac) {
		if (strlen(this->_av[i]) > 0 && this->_av[i][0] == '-') {
			Arguments::Arg *arg = this->find(this->_arguments, this->_av[i], true);
			arg->values.clear();
			for (i = i + 1; i < this->_ac && this->_av[i][0] != '-' ; ++i) {
				arg->values.push_back(std::string(this->_av[i]));
			}
		} else {
			++i;
		}
	}
	return this->check();
}

bool	Arguments::check() {
	for (auto& inc : this->_incompatibleArguments) {
		uint nb = 0;
		for (auto& flag : inc->flags) {
			if (this->find(this->_arguments, flag)) { nb++; }
		}
		if (nb > 1) {
			return false;
		}
	}

	for (auto it = this->_defaultValues.begin() ; it != this->_defaultValues.end() ; ++it) {
		if (!this->find(this->_arguments, (*it)->flag)) {
			this->_arguments.push_back(*it);
			it = this->_defaultValues.erase(it);
		}
	}

	for (auto& exp: this->_expected) {
		const Arguments::Arg *arg = this->find(this->_arguments, exp->flag);
		if (arg) {
			size_t size = arg->values.size();
			if (size < exp->nbmin) {
				std::cout << "not enough" << std::endl;
				return false;
			}
			if (exp->nbmax > 0 && static_cast<size_t>(exp->nbmax) < size) {
				std::cout << "too much" << std::endl;
				return false;
			}
			if (exp->typechecker != nullptr) {
				for (auto& it : arg->values) {
					if (!exp->typechecker(it)) {
						std::cout << "invalid type" << std::endl;
						return false;
					}
				}
			}
		} else if (exp->mandatory && !this->isIncompatible(exp->flag)) {
			std::cout << "mandatory" << std::endl;
			return false;
		}
	}

	for (auto& inc : this->_incompatibleArguments) {
		if (inc->oneMandatory) {
			uint nb = 0;
			for (auto& flag : inc->flags) {
				if (this->find(this->_arguments, flag)) { nb++; }
			}
			if (!nb) {
				std::cout << "incompatible" << std::endl;
				return false;
			}
		}
	}
	return true;
}

const Arguments::Arg*	Arguments::arg(const std::string &flag) const {
	const Arguments::Arg*	argument = this->find(this->_arguments, flag);
	if (!argument) {
		throw std::out_of_range("Argument " + flag + " missing");
	}
	return argument;
}

const Arguments::Arg&	Arguments::operator[](const std::string& flag) const {
	return (*this->arg(flag));
}

std::string Arguments::usage() const {
	std::string usage = std::string("Usage: ") + this->_av[0];
	for (auto& it: this->_expected) {
		if (!this->isIncompatible(it->flag)) {
			usage += ' ';
			if (it->mandatory == false) {
				usage += '[';
			}
			usage += it->flag;
			for (size_t i = 0 ; i < it->nbmin ; ++i) {
				usage += " VALUE";
			}
			if (it->nbmax - it->nbmin == 1) {
				usage += " [VALUE]";
			} else if (it->nbmax - it->nbmin > 1) {
				usage += " [VALUE]+";
			}
			if (it->mandatory == false) {
				usage += ']';
			}
		}
	}
	for (auto& inc : this->_incompatibleArguments) {
		usage += " {";
		if (inc->oneMandatory == false) {
			usage += '[';
		}
		uint i = 0;
		for (auto& flag : inc->flags) {
			const Expected* expected = this->getExpected(flag);
			if (expected) {
				if (i++ > 0) {
					usage += " | ";
				}
				usage += flag;
				for (size_t i = 0 ; i < expected->nbmin ; ++i) {
					usage += " VALUE";
				}
				if (expected->nbmax - expected->nbmin == 1) {
					usage += " [VALUE]";
				} else if (expected->nbmax - expected->nbmin > 1) {
					usage += " [VALUE]+";
				}
			}
		}
		if (inc->oneMandatory == false) {
			usage += ']';
		}
		usage += '}';
	}
	if (!(this->_expected.empty())) {
		usage += "\n\nWith:";
		size_t sizemax = 0;
		for (auto& it : this->_expected) {
			sizemax = MAX(it->flag.length() + it->type.length(), sizemax);
		}
		for (auto& it : this->_expected) {
			usage += "\n" + it->flag + " (" + it->type + "):";
			for (size_t i = it->flag.length() + it->type.length() ; i <= sizemax ; ++i) {
				usage += ' ';
			}
			usage += it->description;
		}
	}
	return usage;
}

Arguments::Arg*	Arguments::find(std::list<Arg*>& args, const std::string &flag, bool create) {
	for (auto& it: args) {
		if (it->flag == flag) {
			return it;
		}
	}
	if (create == true) {
		Arguments::Arg* arg = new Arg(flag);
		args.push_back(arg);
		return arg;
	}
	return nullptr;
}

const Arguments::Arg* Arguments::find(const std::list<Arg*>& args, const std::string &flag) const {
	for (auto& it: args) {
		if (it->flag == flag) {
			return it;
		}
	}
	return nullptr;
}

bool Arguments::isExpected(const std::string& flag) const {
	for (auto& it: this->_expected) {
		if (it->flag == flag) { return true; }
	}
	return false;
}

bool Arguments::isIncompatible(const std::string& flag) const {
	for (auto& arg: this->_incompatibleArguments) {
		for (auto& it: arg->flags) {
			if (it == flag) { return true; }
		}
	}
	return false;
}

const Arguments::Expected* Arguments::getExpected(const std::string& flag) const {
	for (auto& arg: this->_expected) {
		if (arg->flag == flag) { return arg; }
	}
	return nullptr;
}