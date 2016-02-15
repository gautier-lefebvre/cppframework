#include  <algorithm>

#include  "Library/Tool/Macro.hh"
#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Core/Network/Tcp/TcpManager.hh"
#include  "Core/Network/Exception.hh"
#include  "Core/Event/EventManager.hh"

using namespace fwk;

TcpManager::TcpManager(NotifiableThread& input, NotifiableThread& output):
  _servers(),
  _clients(),
  _input(input),
  _output(output)
{}

TcpManager::~TcpManager(void) {}

void TcpManager::clear(void) {
  // close every server
  {
    SCOPELOCK(&(this->_servers));
    for (auto server_it = this->_servers.begin(); server_it != this->_servers.end() ; ++server_it) {
      for (auto& client : (*server_it).clients) {
        // fire onClientClosed event
        this->__fireEvent((*server_it).events.onClientClosed, client);
        // close client
        TcpSocketStream::returnToPool(client);
      }
      // fire onClosed event
      this->__fireEvent((*server_it).events.onClosed, (*server_it).server);
      // close server
      delete (*server_it).server;
      // remove server
      server_it = this->_servers.erase(server_it);
    }
  }

  // close every connection
  {
    SCOPELOCK(&(this->_clients));
    for (auto client_it = this->_clients.begin(); client_it != this->_clients.end() ; ++client_it) {
      // fire onClosed event
      this->__fireEvent((*client_it).events.onClosed, (*client_it).socket);
      // close connection
      TcpSocketStream::returnToPool((*client_it).socket);
      // remove connection
      client_it = this->_clients.erase(client_it);
    }
  }
}

const TcpServer& TcpManager::createServer(uint16_t port) {
  TcpSocket* socket = new TcpSocket();

  try {
    socket->socket();

    const TcpServer* server = nullptr;

    {
      SCOPELOCK(&(this->_servers));

      if (std::find_if(this->_servers.begin(), this->_servers.end(), [=] (const TcpServer& s) -> bool { return s.port == port; }) != this->_servers.end()) {
        throw NetworkException(fmt::format("The port {0} was already associated to a server", port));
      }

      this->_servers.emplace_back(port, socket);
      server = &(this->_servers.back());
    }

    return *server;
  } catch (const NetworkException&) {
    delete socket;
    throw;
  }
}

void TcpManager::run(const TcpServer& server) {
  {
    SCOPELOCK(&(this->_servers));
    auto serverIt = std::find_if(this->_servers.begin(), this->_servers.end(), [&] (const TcpServer& s) -> bool { return s.port == server.port; });

    if (serverIt != this->_servers.end()) {
      try {
        TcpServer& s = (*serverIt);
        s.server->bind(s.port);
        s.server->listen(1024);
        s.active = true;
        INFO(fmt::format("TCP: listening on port {0}", s.port));

      } catch (const std::exception&) {
        delete (*serverIt).server;
        this->_servers.erase(serverIt);
        throw;
      }
    } else {
      throw NetworkException("This server was never created");
    }
  }

  {
    SCOPELOCK(&(this->_input.condition));
    this->_input.condition.notify();
  }
}

void TcpManager::close(uint16_t port) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto server_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const TcpServer& elem) -> bool { return elem.port == port; });

  // if found
  if (server_it != this->_servers.end()) {
    for (auto& client : (*server_it).clients) {
      // fire onClientClosed event
      this->__fireEvent((*server_it).events.onClientClosed, client);
      // send client back to pool
      TcpSocketStream::returnToPool(client);
    }

    // fire server close event
    this->__fireEvent((*server_it).events.onClosed, (*server_it).server);
    // close server
    delete (*server_it).server;

    // remove server
    this->_servers.erase(server_it);
    return;
  }
}

void TcpManager::close(const TcpServer& server) {
  this->close(server.port);
}

void TcpManager::blacklist(uint16_t port, uint32_t addr) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto server_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const TcpServer& elem) -> bool { return elem.port == port; });

  // if found
  if (server_it != this->_servers.end()) {
    (*server_it).blacklist.insert(addr);
    return;
  }
}

const TcpClient& TcpManager::createClient(const std::string& hostname, uint16_t port) {
  TcpSocketStream* socket = TcpSocketStream::getFromPool();

  try {
    socket->init();
    socket->socket();

    const TcpClient* connection = nullptr;

    {
      SCOPELOCK(&(this->_clients));

      if (std::find_if(this->_clients.begin(), this->_clients.end(), [=] (const TcpClient& c) -> bool { return c.hostname == hostname && c.port == port; }) != this->_clients.end()) {
        throw NetworkException(fmt::format("A client to {0}:{1} already exists", hostname, port));
      }

      this->_clients.emplace_back(hostname, port, socket);
      connection = &(this->_clients.back());
    }

    return *connection;
  } catch (const NetworkException&) {
    TcpSocketStream::returnToPool(socket);
    throw;
  }
}

void TcpManager::run(const TcpClient& client) {
  {
    SCOPELOCK(&(this->_clients));
    auto clientIt = std::find_if(this->_clients.begin(), this->_clients.end(), [&] (const TcpClient& c) -> bool { return c.hostname == client.hostname && c.port == client.port; });

    if (clientIt != this->_clients.end()) {
      try {
        TcpClient& c = (*clientIt);
        c.socket->connect(c.hostname, c.port);
        c.active = true;
        INFO(fmt::format("TCP: connected to {0}:{1}", c.hostname, c.port));

      } catch (const std::exception&) {
        delete (*clientIt).socket;
        this->_clients.erase(clientIt);
        throw;
      }
    } else {
      throw NetworkException("This client was never created");
    }
  }

  {
    SCOPELOCK(&(this->_input.condition));
    this->_input.condition.notify();
  }
}

void TcpManager::close(const std::string& hostname, uint16_t port) {
  SCOPELOCK(&(this->_clients));

  // find the connection
  auto client_it = std::find_if(this->_clients.begin(), this->_clients.end(),
    [&] (const TcpClient& elem) -> bool { return elem.hostname == hostname && elem.port == port; });

  // if found
  if (client_it != this->_clients.end()) {
    // fire close event
    this->__fireEvent((*client_it).events.onClosed, (*client_it).socket);
    // close socket
    TcpSocketStream::returnToPool((*client_it).socket);

    // remove server
    this->_clients.erase(client_it);
    return;
  }
}

void TcpManager::close(const TcpClient& connection) {
  this->close(connection.hostname, connection.port);
}

void TcpManager::push(TcpSocketStream* socket, const void* data, size_t size) {
  if (socket != nullptr) {
    try {
      socket->push(data, size);

      {
        SCOPELOCK(&(this->_output.condition));
        this->_output.condition.notify();
      }
    } catch (const NetworkException& e) {
      // find server
      {
        SCOPELOCK(&(this->_servers));
        for (auto& server : this->_servers) {
          auto client_it = std::find(server.clients.begin(), server.clients.end(), socket);
          if (client_it != server.clients.end()) {
            this->__onIOException(server.events.onClientClosed, socket, e.what());
            server.clients.erase(client_it);
            return;
          }
        }
      }

      // find connection
      {
        SCOPELOCK(&(this->_clients));
        auto client_it = std::find_if(this->_clients.begin(), this->_clients.end(),
          [&] (const TcpClient& elem) -> bool { return elem.socket == socket; });

        if (client_it != this->_clients.end()) {
          this->__onIOException((*client_it).events.onClosed, socket, e.what());
          this->_clients.erase(client_it);
          return;
        }
      }

      // if no server / connection found
      // close the socket
      // THIS IS NOT NORMAL, THIS MEANS A POINTER TO THE SOCKET IS STILL USED IN THE APPLICATION
      // EVEN THOUGH IT WAS CLOSED BEFORE
      TcpSocketStream::returnToPool(socket);
    }
  }
}

void TcpManager::push(TcpSocketStream* socket, const ByteArray* bytearray) {
  if (bytearray != nullptr) {
    this->push(socket, bytearray->getBytes(), bytearray->getSize());
  }
}

void TcpManager::fillSetRead(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.active) {
        server.server->addToSet(fdset, fdmax);
        nb++;
        for (auto& client : server.clients) {
          client->addToSet(fdset, fdmax);
          nb++;
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto& client : this->_clients) {
      if (client.active) {
        client.socket->addToSet(fdset, fdmax);
        nb++;
      }
    }
  }
}

void TcpManager::fillSetWrite(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.active) {
        for (auto& client : server.clients) {
          if (client->hasDataToSend()) {
            client->addToSet(fdset, fdmax);
            nb++;
          }
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto& client : this->_clients) {
      if (client.active && client.socket->hasDataToSend()) {
        client.socket->addToSet(fdset, fdmax);
        nb++;
      }
    }
  }
}

void TcpManager::send(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.active) {
        for (auto client_it = server.clients.begin() ; client_it != server.clients.end() ; ++client_it) {
          if ((*client_it)->isset(set)) {
            try {
              (*client_it)->send();
            } catch (const NetworkException& e) {
              // fire client closed event + close client
              this->__onIOException(server.events.onClientClosed, *client_it, e.what());

              // remove client
              client_it = server.clients.erase(client_it);
            }
          }
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto client_it = this->_clients.begin() ; client_it != this->_clients.end() ; ++client_it) {
      if ((*client_it).active && (*client_it).socket->isset(set)) {
        try {
          (*client_it).socket->send();
        } catch (const NetworkException& e) {
          // fire closed event + close connection
          this->__onIOException((*client_it).events.onClosed, (*client_it).socket, e.what());

          // remove connection
          client_it = this->_clients.erase(client_it);
        }
      }
    }
  }
}

void TcpManager::recv(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.active) {
        // accept new clients (except if in blacklist or if not in accept list if not empty)
        if (server.server->isset(set)) {
          TcpSocketStream* client = TcpSocketStream::getFromPool();
          try {
            uint32_t remote_ip = server.server->accept(client);

            // if accept list is not empty and remote ip not in it
            if (!(server.accept.empty()) && std::find(server.accept.begin(), server.accept.end(), remote_ip) == server.accept.end()) {
              throw NetworkException("Unauthorized IP attempted to connect");
            }

            // if blacklisted ip
            if (!(server.blacklist.empty()) && std::find(server.blacklist.begin(), server.blacklist.end(), remote_ip) != server.blacklist.end()) {
              throw NetworkException("Blacklisted IP attempted to connect");
            }

            // get bytearrays from pool
            client->init();

            // add client
            server.clients.push_back(client);

            // fire onAccept event
            this->__fireEvent(server.events.onAccept, client);

          } catch (const NetworkException& e) {
            WARNING(e.what());
            TcpSocketStream::returnToPool(client);
          }
        }

        // receive data from clients
        for (auto client_it = server.clients.begin() ; client_it != server.clients.end() ; ++client_it) {
          if ((*client_it)->isset(set)) {
            try {
              // receive data
              (*client_it)->recv();

              // fire onReceivedData event
              this->__fireEvent(server.events.onReceivedData, *client_it);
            } catch (const NetworkException& e) {
              // fire onClientClosed event + close client
              this->__onIOException(server.events.onClientClosed, *client_it, e.what());

              // remove client
              client_it = server.clients.erase(client_it);
            }
          }
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto client_it = this->_clients.begin() ; client_it != this->_clients.end() ; ++client_it) {
      if ((*client_it).active && (*client_it).socket->isset(set)) {
        try {
          // receive data
          (*client_it).socket->recv();

          // fire onReceivedData event
          this->__fireEvent((*client_it).events.onReceivedData, (*client_it).socket);
        } catch (const NetworkException& e) {
          // fire onClosed event + close socket
          this->__onIOException((*client_it).events.onClosed, (*client_it).socket, e.what());

          // remove connection
          client_it = this->_clients.erase(client_it);
        }
      }
    }
  }
}

void TcpManager::__onIOException(EventHandle* event, TcpSocketStream* socket, const std::string&) {
  // fire closed event
  this->__fireEvent(event, socket);

  // close socket
  TcpSocketStream::returnToPool(socket);
}

void TcpManager::__fireEvent(EventHandle* event, TcpSocketStream* socket) const {
  TcpSocketStreamEventArgs* ssargs = TcpSocketStreamEventArgs::getFromPool(socket);
  event->fireSync(ssargs);
}

void TcpManager::__fireEvent(EventHandle* event, TcpSocket* socket) const {
  TcpSocketEventArgs* sargs = TcpSocketEventArgs::getFromPool(socket);
  event->fireSync(sargs);
}
