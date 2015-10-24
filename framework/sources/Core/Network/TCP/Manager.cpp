#include  <algorithm>

#include  "Library/Tool/Macro.hh"
#include  "Core/Network/TCP/Manager.hh"
#include  "Core/Event/Manager.hh"

Core::Network::TCP::Manager::Manager(NotifiableThread& input, NotifiableThread& output):
  _servers(),
  _connections(),
  _input(input),
  _output(output)
{}

Core::Network::TCP::Manager::~Manager(void) {}

void Core::Network::TCP::Manager::clear(void) {
  // close every server
  {
    SCOPELOCK(&(this->_servers));
    for (auto serv_it = this->_servers.begin(); serv_it != this->_servers.end() ; ++serv_it) {
      for (auto& client : (*serv_it).clients) {
        // fire onClientClosed event
        this->__fireEvent((*serv_it).events.onClientClosed, client);
        // close client
        Core::Network::TCP::SocketStream::returnToPool(client);
      }
      // fire onClosed event
      this->__fireEvent((*serv_it).events.onClosed, (*serv_it).server);
      // close server
      Core::Network::TCP::Socket::returnToPool((*serv_it).server);
      // remove server
      serv_it = this->_servers.erase(serv_it);
    }
  }

  // close every connection
  {
    SCOPELOCK(&(this->_connections));
    for (auto conn_it = this->_connections.begin(); serv_it != this->_connections.end() ; ++serv_it) {
      // fire onClosed event
      this->__fireEvent((*conn_it).events.onClosed, (*conn_it).socket);
      // close connection
      Core::Network::TCP::SocketStream::returnToPool((*conn_it).socket);
      // remove connection
      conn_it = this->_connections.erase(conn_it);
    }
  }
}

const Core::Network::TCP::Manager::ServerClients& Core::Network::TCP::Manager::bind(uint16_t port, const std::set<uint32_t>& accept, const std::set<uint32_t>& blacklist) {
  Core::Network::TCP::Socket* socket = Core::Network::TCP::Socket::getFromPool();

  try {
    socket->socket();
    socket->bind(port);
    socket->listen(1024);
  } catch (const Core::Network::Exception& e) {
    Core::Network::TCP::Socket::returnToPool(socket);
    throw e;
  }

  const Core::Network::TCP::Manager::ServerClients* server = nullptr;

  {
    SCOPELOCK(&(this->_servers));
    this->_servers.emplace_back(port, socket, accept, blacklist);
    server = &(this->_servers.back());
  }

  {
    SCOPELOCK(&(this->_input.condition));
    this->_input.condition.notify();
  }

  return *server;
}

void Core::Network::TCP::Manager::close(uint16_t port) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto serv_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const Core::Network::TCP::Manager::ServerClients& elem) -> bool { return elem.port == port; });

  // if found
  if (serv_it != this->_servers.end()) {
    // close every client
    Core::Network::TCP::EventArgs::SocketStreamArgs* ssargs = nullptr;

    for (auto& client : (*serv_it).clients) {
      // fire onClientClosed event
      this->__fireEvent((*serv_it).events.onClientClosed, client);

      // send client back to pool
      Core::Network::TCP::SocketStream::returnToPool(this->socket);
    }

    // fire server close event
    this->__fireEvent((*serv_it).events.onClosed, (*serv_it).server);

    // close server
    Core::Network::TCP::Socket::returnToPool((*serv_it).server);

    // remove server
    this->_servers.erase(serv_it);
    return;
  }
}

void Core::Network::TCP::Manager::close(const Core::Network::TCP::Manager::ServerClients& server) {
  this->close(server.port);
}

void Core::Network::TCP::Manager::blacklist(uint16_t port, uint32_t addr) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto serv_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const Core::Network::TCP::Manager::ServerClients& elem) -> bool { return elem.port == port });

  // if found
  if (serv_it != this->_servers.end()) {
    (*serv_it).blacklist.insert(addr);
    return;
  }
}

const RemoteConnection& Core::Network::TCP::Manager::connect(const std::string& hostname, uint16_t port) {
  Core::Network::TCP::SocketStream* socket = Core::Network::TCP::SocketStream::getFromPool();

  try {
    socket->socket();
    socket->connect(hostname, port);
  } catch (const Core::Network::Exception& e) {
    Core::Network::TCP::SocketStream::returnToPool(socket);
    throw e;
  }

  const Core::Network::TCP::Manager::RemoteConnection* connection = nullptr;

  {
    SCOPELOCK(&(this->_connections));
    this->_connections.emplace_back(hostname, port, socket);
    connection = &(this->_connections.back());
  }

  {
    SCOPELOCK(&(this->_input.condition));
    this->_input.condition.notify();
  }

  return *connection;
}

void Core::Network::TCP::Manager::close(const std::string& hostname, uint16_t port) {
  SCOPELOCK(&(this->_connections));

  // find the connection
  auto conn_it = std::find_if(this->_connections.begin(), this->_connections.end(),
    [&] (const Core::Network::TCP::Manager::RemoteConnection& elem) -> bool { return elem.hostname == hostname && elem.port == port; });

  // if found
  if (conn_it != this->_connections.end()) {
    // fire close event
    this->__fireEvent((*conn_it).events.onClosed, (*conn_it).socket);

    // close socket
    Core::Network::TCP::Socket::returnToPool((*conn_it).socket);

    // remove server
    this->_connections.erase(conn_it);
    return;
  }
}

void Core::Network::TCP::Manager::close(const Core::Network::TCP::Manager::RemoteConnection& connection) {
  this->close(connection.hostname, connection.port);
}

void Core::Network::TCP::Manager::push(Core::Network::TCP::SocketStream* socket, void* data, size_t size) {
  if (socket != nullptr) {
    try {
      socket->push(data, size);

      {
        SCOPELOCK(&(this->_output.condition));
        this->_output.condition.notify();
      }
    } catch (const Core::Network::Exception& e) {
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
        SCOPELOCK(&(this->_connections));
        auto conn_it = std::find_if(this->_connections.begin(), this->_connections.end(),
          [&] (const Core::Network::TCP::Manager::RemoteConnection& elem) -> bool { return elem.socket == socket; });

        if (conn_it != this->_connections.end()) {
          this->__onIOException((*conn_it).onClosed, socket, e.what());
          this->_connections.erase(conn_it);
          return;
        }
      }

      // if no server / connection found
      // close the socket
      // THIS IS NOT NORMAL, THIS MEANS A POINTER TO THE SOCKET IS STILL USED IN THE APPLICATION
      // EVEN THOUGH IT WAS CLOSED BEFORE
      Core::Network::TCP::SocketStream::returnToPool(socket);
    }
  }
}

void Core::Network::TCP::Manager::push(Core::Network::TCP::SocketStream* socket, const ByteArray* bytearray) {
  if (bytearray != nullptr) {
    this->push(socket, bytearray->getBytes(), bytearray->getSize());
  }
}

void Core::Network::TCP::Manager::fillSetRead(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      server.server.addToSet(fdset, fdmax);
      nb++;
      for (auto& client : server.clients) {
        client->addToSet(fdset, fdmax);
        nb++;
      }
    }
  }

  {
    SCOPELOCK(&(this->_connections));
    for (auto& connection : this->_connections) {
      connection.socket->addToSet(fdset, fdmax);
      nb++;
    }
  }
}

void Core::Network::TCP::Manager::fillSetWrite(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      for (auto& client : server.clients) {
        if (client->hasDataToSend()) {
          client->addToSet(fdset, fdmax);
          nb++;
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_connections));
    for (auto& connection : this->_connections) {
      if (connection.socket->hasDataToSend()) {
        connection.socket->addToSet(fdset, fdmax);
        nb++;
      }
    }
  }
}

void Core::Network::TCP::Manager::send(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      for (auto client_it = server.clients.begin() ; client_it != server.clients.end() ; ++client_it) {
        if ((*client_it)->isset(set)) {
          try {
            (*client_it)->send();
          } catch (const Core::Network::Exception& e) {
            // fire client closed event + close client
            this->__onIOException(server.events.onClientClosed, *client_it, e.what());

            // remove client
            client_it = server.clients.erase(client_it);
          }
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_connections));
    for (auto conn_it = this->_connections.begin() ; conn_it != this->_connections.end() ; ++conn_it) {
      if ((*conn_it).socket->isset(set)) {
        try {
          (*conn_it).socket->send();
        } catch (const Core::Network::Exception& e) {
          // fire closed event + close connection
          this->__onIOException((*conn_it).events.onClosed, (*conn_it).socket, e.what());

          // remove connection
          conn_it = this->_connections.erase(conn_it);
        }
      }
    }
  }
}

void Core::Network::TCP::Manager::recv(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      // accept new clients (except if in blacklist or if not in accept list it not empty)
      if (server.server->isset(set)) {
        Core::Network::TCP::SocketStream* client = Core::Network::TCP::SocketStream::getFromPool();
        try {
          uint32_t remote_ip = server.server->accept(client);

          // if accept list is not empty and remote ip not in it
          if (!(server.accept.empty()) && std::find(server.accept.begin(), server.accept.end(), remote_ip) == server.accept.end()) {
            throw Core::Network::Exception("Unauthorized IP attempted to connect");
          }

          // if blacklisted ip
          if (!(server.blacklist.empty()) && std::find(server.blacklist.begin(), server.blacklist.end(), remote_ip) != server.blacklist.end()) {
            throw Core::Network::Exception("Blacklisted IP attempted to connect");
          }

          // add client
          server.clients.push_back(client);

          // fire onAccept event
          this->__fireEvent(server.events.onAccept, client);

        } catch (const Core::Network::Exception& e) {
          WARNING(e.what());
          Core::Network::TCP::SocketStream::returnToPool(client);
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
          } catch (const Core::Network::Exception& e) {
            // fire onClientClosed event + close client
            this->__onIOException(server.events.onClientClosed, *client_it, e.what());

            // remove client
            client_it = server.clients.erase(client_it);
          }
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_connections));
    for (auto conn_it = this->_connections.begin() ; conn_it != this->_connections.end() ; ++conn_it) {
      if ((*conn_it).socket->isset(set)) {
        try {
          // receive data
          (*conn_it).socket->recv();

          // fire onReceivedData event
          this->__fireEvent((*conn_it).events.onReceivedData, (*conn_it).socket);
        } catch (const Core::Network::Exception& e) {
          // fire onClosed event + close socket
          this->__onIOException((*conn_it).events.onClosed, (*conn_it).socket, e.what());

          // remove connection
          conn_it = this->_connections.erase(conn_it);
        }
      }
    }
  }
}

void Core::Network::TCP::Manager::__onIOException(Core::Event::Event* event, Core::Network::TCP::SocketStream* socket, const std::string&) {
  // fire closed event
  this->__fireEvent(event, socket);

  // close socket
  Core::Network::TCP::SocketStream::returnToPool(socket);
}

void Core::Network::TCP::Manager::__fireEvent(Core::Event::Event* event, Core::Network::TCP::SocketStream* socket) {
  Core::Network::TCP::EventArgs::SocketStreamArgs* ssargs = Core::Network::TCP::EventArgs::SocketStreamArgs::getFromPool(socket);
  Core::Event::Manager::get().fireEventSync(event, ssargs);
}

/**
 *  ServerClients
 */

Core::Network::TCP::Manager::ServerClients::ServerClients(uint16_t port, Core::Network::TCP::Socket* server, const std::set<uint32_t>& accept, const std::set<uint32_t>& blacklist):
  Threading::Lock(),
  port(port),
  server(server),
  clients(),
  accept(accept),
  blacklist(blacklist),
  events({
    Core::Event::Event::getFromPool(),
    Core::Event::Event::getFromPool(),
    Core::Event::Event::getFromPool(),
    Core::Event::Event::getFromPool()
  })
{
  Core::Event::Manager& eventManager = Core::Event::Manager::get();
  eventManager.registerEvent(this->events.onAccept);
  eventManager.registerEvent(this->events.onReceivedData);
  eventManager.registerEvent(this->events.onClientClosed);
  eventManager.registerEvent(this->events.onClosed);
}

Core::Network::TCP::Manager::ServerClients::~ServerClients(void) {
  Core::Event::Manager& eventManager = Core::Event::Manager::get();
  eventManager.unregisterEvent(this->events.onAccept);
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClientClosed);
  eventManager.unregisterEvent(this->events.onClosed);
}

/**
 *  RemoteConnection
 */

Core::Network::TCP::Manager::RemoteConnection::RemoteConnection(const std::string& hostname, uint16_t port, Core::Network::TCP::SocketStream* socket):
  hostname(hostname),
  port(port),
  socket(socket),
  events({
    Core::Event::Event::getFromPool(),
    Core::Event::Event::getFromPool()
  })
{
  Core::Event::Manager& eventManager = Core::Event::Manager::get();
  eventManager.registerEvent(this->events.onReceivedData);
  eventManager.registerEvent(this->events.onClosed);
}

Core::Network::TCP::Manager::RemoteConnection::~RemoteConnection(void) {
  Core::Event::Manager& eventManager = Core::Event::Manager::get();
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClosed);
}

/**
 *  SocketStreamArgs
 */

Core::Network::TCP::EventArgs::SocketStreamArgs::SocketStreamArgs(void):
  socket(nullptr)
{}

void Core::Network::TCP::EventArgs::SocketStreamArgs::reinit(void) {
  this->socket = nullptr;
}

void Core::Network::TCP::EventArgs::SocketStreamArgs::init(Core::Network::TCP::SocketStream* socket) {
  this->socket = socket;
}

void Core::Network::TCP::EventArgs::SocketStreamArgs::cleanup(void) {
  Core::Network::TCP::EventArgs::SocketStreamArgs::returnToPool(this);
}