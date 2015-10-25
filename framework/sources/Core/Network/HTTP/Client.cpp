#include  <curl/curl.h>
#include  <openssl/ssl.h>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/HTTP/Client.hh"
#include  "Core/Network/Exception.hh"

Core::Network::HTTP::Client::Client(void):
  Threading::Lock(),
  AEndable(),
  _userAgent(""),
  _connections()
{}

Core::Network::HTTP::Client::~Client(void) {
  this->end();
}

void  Core::Network::HTTP::Client::end(void) {
  SCOPELOCK(this);
  if (!(this->mustEnd())) {
    this->mustEnd(true);

    for (auto &connection : this->_connections) {
      connection->end();
      delete connection;
    }

    curl_global_cleanup();
    sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
  }
}

void  Core::Network::HTTP::Client::init(const std::string& user_agent) {
  this->_userAgent = user_agent;
  if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
    throw Core::Network::Exception("cURL library initialization failed");
  }
}

Core::Network::HTTP::Connection * Core::Network::HTTP::Client::getConnectionByHostPort(const std::string& host, uint16_t port, uint16_t secureport, bool create) {
  SCOPELOCK(this);
  for (auto &connection : this->_connections) {
    if (connection->getHost() == host && connection->getPort() == port && connection->getSecurePort() == secureport) {
      return connection;
    }
  }
  if (create) {
    Core::Network::HTTP::Connection *connection = nullptr;
    try {
      connection = new Core::Network::HTTP::Connection(host, port, secureport, this->_userAgent);
      this->_connections.push_back(connection);
      connection->run();
      return connection;
    } catch (const Core::Network::Exception& e) {
      delete connection;
      throw e;
    }
  } else {
    return nullptr;
  }
}

void  Core::Network::HTTP::Client::sendRequest(Core::Network::HTTP::Request *request, const std::string& host, uint16_t port, uint16_t secureport) {
  try {
    this->getConnectionByHostPort(host, port, secureport)->addRequest(request);
  } catch (const Core::Network::Exception& e) {
    CRITICAL(e.what());
    Core::Network::HTTP::Request::returnToPool(request);
  }
}

void  Core::Network::HTTP::Client::sendRequest(Core::Network::HTTP::Request *request, Core::Network::HTTP::Connection *connection) {
  if (connection != nullptr) {
    connection->addRequest(request);
  } else {
    Core::Network::HTTP::Request::returnToPool(request);
  }
}