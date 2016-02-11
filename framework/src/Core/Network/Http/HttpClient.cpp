#include  <curl/curl.h>
#include  <openssl/ssl.h>

#include  "Library/Tool/Logger.hpp"
#include  "Core/Network/Http/HttpClient.hh"
#include  "Core/Network/Http/HttpPipeliningConnection.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

const std::string HttpClient::defaultUserAgent = std::string("cppframework user agent");

HttpClient::HttpClient(void):
  Lockable(),
  AEndable(),
  _userAgent(""),
  _connections()
{}

HttpClient::~HttpClient(void) {
  this->end();
}

void  HttpClient::end(void) {
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

void  HttpClient::init(const std::string& user_agent) {
  this->_userAgent = user_agent;
  if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
    throw NetworkException("cURL library initialization failed");
  }
}

HttpConnection * HttpClient::initConnection(const std::string& host, uint16_t port, HttpProtocol protocol, bool enablePipelining) {
  SCOPELOCK(this);

  // if connection found -> return connection (this does not change the pipelining parameter !)
  for (auto &connection : this->_connections) {
    if (connection->getHost() == host && connection->getPort() == port && connection->getProtocol() == protocol) {
      return connection;
    }
  }

  // if connection not found -> create new connection
  HttpConnection *connection = nullptr;
  try {
    if (!enablePipelining) {
      connection = new HttpConnection(host, port, protocol, this->_userAgent);
    } else {
      connection = new HttpPipeliningConnection(host, port, protocol, this->_userAgent);
    }

    this->_connections.push_back(connection);
    connection->run();
    return connection;
  } catch (const NetworkException& e) {
    delete connection;
    throw e;
  }
}

void  HttpClient::sendRequest(HttpConnection *connection, HttpRequest *request) {
  if (connection != nullptr) {
    connection->addRequest(request);
  } else {
    HttpRequest::returnToPool(request);
  }
}
