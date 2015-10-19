#include	<strings.h>

#include	"Core/Network/HTTP/AMessage.hh"

Core::Network::HTTP::AMessage::AMessage(void):
	Factory::AFactored(),
	version("HTTP/1.1"),
	headers(),
	body(nullptr)
{}

Core::Network::HTTP::AMessage::~AMessage(void) {
	this->reinit();
}

void	Core::Network::HTTP::AMessage::reinit(void) {
	if (this->body != nullptr) {
		ByteArray::Pool::get().remove(this->body);
	}
	this->body = nullptr;
}

void	Core::Network::HTTP::AMessage::init(void) {
	if (this->body == nullptr) {
		this->body = ByteArray::Pool::get().create();
	}
}

const std::string&	Core::Network::HTTP::AMessage::getHeader(const std::string& header) const {
	for (auto &it : this->headers) {
		if (!strcasecmp(it.first.c_str(), header.c_str())) {
			return it.second;
		}
	}
	throw std::out_of_range("HTTP Message does not have header " + header);
}

bool	Core::Network::HTTP::AMessage::hasHeader(const std::string& header) const {
	for (auto &it : this->headers) {
		if (!strcasecmp(it.first.c_str(), header.c_str())) {
			return true;
		}
	}
	return false;
}
