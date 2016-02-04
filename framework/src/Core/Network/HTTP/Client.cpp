#include  <curl/curl.h>
#include  <openssl/ssl.h>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/HTTP/Client.hh"
#include  "Core/Network/HTTP/PipeliningConnection.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

const std::string Core::Network::HTTP::Client::defaultUserAgent = std::string("cppframework user agent");

Core::Network::HTTP::Client::Client(void):
  Threading::Lockable(),
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

Core::Network::HTTP::Connection * Core::Network::HTTP::Client::initConnection(const std::string& host, uint16_t port, Core::Network::HTTP::Protocol protocol, bool enablePipelining) {
  SCOPELOCK(this);

  // if connection found -> return connection (this does not change the pipelining parameter !)
  for (auto &connection : this->_connections) {
    if (connection->getHost() == host && connection->getPort() == port && connection->getProtocol() == protocol) {
      return connection;
    }
  }

  // if connection not found -> create new connection
  Core::Network::HTTP::Connection *connection = nullptr;
  try {
    if (!enablePipelining) {
      connection = new Core::Network::HTTP::Connection(host, port, protocol, this->_userAgent);
    } else {
      connection = new Core::Network::HTTP::PipeliningConnection(host, port, protocol, this->_userAgent);
    }

    this->_connections.push_back(connection);
    connection->run();
    return connection;
  } catch (const Core::Network::Exception& e) {
    delete connection;
    throw e;
  }
}

void  Core::Network::HTTP::Client::sendRequest(Core::Network::HTTP::Connection *connection, Core::Network::HTTP::Request *request) {
  if (connection != nullptr) {
    connection->addRequest(request);
  } else {
    Core::Network::HTTP::Request::returnToPool(request);
  }
}
