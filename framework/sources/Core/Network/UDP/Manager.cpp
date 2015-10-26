#include  <algorithm>

#include  "Library/Tool/Macro.hh"
#include  "Library/Threading/Condition.hpp"
#include  "Core/Network/UDP/Manager.hh"
#include  "Core/Network/Exception.hh"
#include  "Core/Event/Manager.hh"

Core::Network::UDP::Manager::Manager(Threading::NotifiableThread& input, Threading::NotifiableThread& output):
  _servers(),
  _clients(),
  _input(input),
  _output(output)
{}

Core::Network::UDP::Manager::~Manager(void) {}

void Core::Network::UDP::Manager::clear(void) {
  // close every server
  {
    SCOPELOCK(&(this->_servers));
    for (auto server_it = this->_servers.begin(); server_it != this->_servers.end() ; ++server_it) {
      for (auto& client : (*server_it).clients) {
        // fire onClientClosed event
        this->__fireEvent((*server_it).events.onClientClosed, client);
        // close client
        Core::Network::UDP::SocketClient::returnToPool(client);
      }
      // fire onClosed event
      this->__fireEvent((*server_it).events.onClosed, (*server_it).server);
      // close server
      Core::Network::UDP::SocketServer::returnToPool((*server_it).server);
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
      Core::Network::UDP::SocketStream::returnToPool((*client_it).socket);
      // remove connection
      client_it = this->_clients.erase(client_it);
    }
  }
}

const Core::Network::UDP::Manager::Server& Core::Network::UDP::Manager::bind(uint16_t port, const std::set<uint32_t>& accept, const std::set<uint32_t>& blacklist) {
  Core::Network::UDP::SocketServer* socket = Core::Network::UDP::SocketServer::getFromPool();

  try {
    socket->socket();
    socket->bind(port);
  } catch (const Core::Network::Exception& e) {
    Core::Network::UDP::SocketServer::returnToPool(socket);
    throw e;
  }

  const Core::Network::UDP::Manager::Server* server = nullptr;

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

void Core::Network::UDP::Manager::close(uint16_t port) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto server_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const Core::Network::UDP::Manager::Server& elem) -> bool { return elem.port == port; });

  // if found
  if (server_it != this->_servers.end()) {
    for (auto& client : (*server_it).clients) {
      // fire onClientClosed event
      this->__fireEvent((*server_it).events.onClientClosed, client);
      // send client back to pool
      Core::Network::UDP::SocketClient::returnToPool(client);
    }

    // fire server close event
    this->__fireEvent((*server_it).events.onClosed, (*server_it).server);
    // close server
    Core::Network::UDP::SocketServer::returnToPool((*server_it).server);

    // remove server
    this->_servers.erase(server_it);
    return;
  }
}

void Core::Network::UDP::Manager::close(const Core::Network::UDP::Manager::Server& server) {
  this->close(server.port);
}

void Core::Network::UDP::Manager::blacklist(uint16_t port, uint32_t addr) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto server_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const Core::Network::UDP::Manager::Server& elem) -> bool { return elem.port == port; });

  // if found
  if (server_it != this->_servers.end()) {
    (*server_it).blacklist.insert(addr);
    return;
  }
}

const Core::Network::UDP::Manager::Client& Core::Network::UDP::Manager::connect(const std::string& hostname, uint16_t port) {
  Core::Network::UDP::SocketStream* socket = Core::Network::UDP::SocketStream::getFromPool();

  try {
    socket->socket();
    socket->init(hostname, port);
  } catch (const Core::Network::Exception& e) {
    Core::Network::UDP::SocketStream::returnToPool(socket);
    throw e;
  }

  const Core::Network::UDP::Manager::Client* client = nullptr;

  {
    SCOPELOCK(&(this->_clients));
    this->_clients.emplace_back(hostname, port, socket);
    client = &(this->_clients.back());
  }

  {
    SCOPELOCK(&(this->_input.condition));
    this->_input.condition.notify();
  }

  return *client;
}

void Core::Network::UDP::Manager::close(const std::string& hostname, uint16_t port) {
  SCOPELOCK(&(this->_clients));

  // find the connection
  auto client_it = std::find_if(this->_clients.begin(), this->_clients.end(),
    [&] (const Core::Network::UDP::Manager::Client& elem) -> bool { return elem.hostname == hostname && elem.port == port; });

  // if found
  if (client_it != this->_clients.end()) {
    // fire close event
    this->__fireEvent((*client_it).events.onClosed, (*client_it).socket);
    // close socket
    Core::Network::UDP::SocketStream::returnToPool((*client_it).socket);

    // remove connection
    this->_clients.erase(client_it);
    return;
  }
}

void Core::Network::UDP::Manager::close(const Core::Network::UDP::Manager::Client& connection) {
  this->close(connection.hostname, connection.port);
}

void Core::Network::UDP::Manager::push(Core::Network::UDP::ASocketIO* socket, const void* data, size_t size) {
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
        SCOPELOCK(&(this->_clients));
        auto client_it = std::find_if(this->_clients.begin(), this->_clients.end(),
          [&] (const Core::Network::UDP::Manager::Client& elem) -> bool { return elem.socket == socket; });

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
      CRITICAL("Invalid UDP socket still used inside application.");
      delete socket; // prevent leaks, but can lead to other bugs (this is one in itself anyway)
    }
  }
}

void Core::Network::UDP::Manager::push(Core::Network::UDP::ASocketIO* socket, const ByteArray* bytearray) {
  if (bytearray != nullptr) {
    this->push(socket, bytearray->getBytes(), bytearray->getSize());
  }
}

void Core::Network::UDP::Manager::fillSetRead(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      server.server->addToSet(fdset, fdmax);
      nb++;
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto& connection : this->_clients) {
      connection.socket->addToSet(fdset, fdmax);
      nb++;
    }
  }
}

void Core::Network::UDP::Manager::fillSetWrite(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    // for each server -> add to set if at least one client has data to send
    for (auto& server : this->_servers) {
      for (auto& client : server.clients) {
        if (client->hasDataToSend()) {
          server.server->addToSet(fdset, fdmax);
          nb++;
          break;
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto& connection : this->_clients) {
      if (connection.socket->hasDataToSend()) {
        connection.socket->addToSet(fdset, fdmax);
        nb++;
      }
    }
  }
}

void Core::Network::UDP::Manager::send(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.server->isset(set)) {
        for (auto client_it = server.clients.begin() ; client_it != server.clients.end() ; ++client_it) {
          if ((*client_it)->hasDataToSend()) {
            try {
              server->sendto(*client_it);
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
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto client_it = this->_clients.begin() ; client_it != this->_clients.end() ; ++client_it) {
      if ((*client_it).socket->isset(set)) {
        try {
          (*client_it).socket->sendto();
        } catch (const Core::Network::Exception& e) {
          // fire closed event + close connection
          this->__onIOException((*client_it).events.onClosed, (*client_it).socket, e.what());
          // remove connection
          client_it = this->_clients.erase(client_it);
        }
      }
    }
  }
}

void Core::Network::UDP::Manager::recv(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    sockaddr_in addr;

    for (auto& server : this->_servers) {
      if (server.server->isset(set)) {
        memset(&addr, 0, sizeof(sockaddr_in)); // reinit the addr in order not to remove a valid client because of old data
        ByteArray* datagram = nullptr;
        bool success = true; // true -> recvfrom did not except / false -> recvfrom did except

        try {
          datagram = server.server->recvfrom(addr);
        } catch (const Core::Network::Exception& e) {
          WARNING(e.what());
          continue;
        }

        // find client
        auto client_it = std::find_if(server.clients.begin(), server.clients.end(),
          [&] (const Core::Network::UDP::SocketClient*& socket) -> bool { return *socket == addr; });

        if (success) {
          try {
            // if client found -> add datagram
            // if client not found -> check if client not blacklisted / accepted
            //                        add client + add datagram
            if (client_it != server.clients.end()) {
              (*client_it)->received(datagram);
            } else {
              uint32_t remote_ip = static_cast<uint32_t>(sin.sin_addr.s_addr);

              // if accept list is not empty and remote ip not in it
              if (!(server.accept.empty()) && std::find(server.accept.begin(), server.accept.end(), remote_ip) == server.accept.end()) {
                throw Core::Network::Exception("Unauthorized IP attempted to send data (UDP)");
              }

              // if blacklisted ip
              if (!(server.blacklist.empty()) && std::find(server.blacklist.begin(), server.blacklist.end(), remote_ip) != server.blacklist.end()) {
                throw Core::Network::Exception("Blacklisted IP attempted to send data (UDP)");
              }

              Core::Network::UDP::SocketClient* client = Core::Network::UDP::SocketClient::getFromPool(addr);

              try {
                client->received(datagram);
              } catch (const Core::Network::Exception& e) {
                WARNING("Something went wrong, a single datagram could not overflow the limit");
                Core::Network::UDP::SocketClient::returnToPool(client);
                throw e;
              }

              server.clients.push_back(client);
              // fire new client event
              this->__fireEvent(server.events.onNewClient, client);
              // fire data received event
              this->__fireEvent(server.events.onReceivedData, client);
            }
          } catch (const Core::Network::Exception& e) {
            ByteArray::returnToPool(datagram);
            WARNING(e.what());
          }
        } else {
          // if recvfrom excepted and client in list -> remove client
          if (client_it != server.clients.end()) {
            // fire onClientClosed event
            this->__fireEvent(server.events.onClientClosed, *client_it);
            // remove client
            client_it = server.clients.erase(client_it);
          }
        }
      }
    }
  }

  {
    SCOPELOCK(&(this->_clients));
    for (auto client_it = this->_clients.begin() ; client_it != this->_clients.end() ; ++client_it) {
      if ((*client_it).socket->isset(set)) {
        try {
          // receive data
          (*client_it).socket->recvfrom();
          // fire onReceivedData event
          this->__fireEvent((*client_it).events.onReceivedData, (*client_it).socket);
        } catch (const Core::Network::Exception& e) {
          // fire onClosed event + close socket
          this->__onIOException((*client_it).events.onClosed, (*client_it).socket, e.what());
          // remove connection
          client_it = this->_clients.erase(client_it);
        }
      }
    }
  }
}

void Core::Network::UDP::Manager::__onIOException(Core::Event::Event* event, Core::Network::UDP::SocketClient* socket, const std::string&) {
  // fire closed event
  this->__fireEvent(event, socket);

  // close socket
  Core::Network::UDP::SocketClient::returnToPool(socket);
}

void Core::Network::UDP::Manager::__onIOException(Core::Event::Event* event, Core::Network::UDP::SocketStream* socket, const std::string&) {
  // fire closed event
  this->__fireEvent(event, socket);

  // close socket
  Core::Network::UDP::SocketStream::returnToPool(socket);
}

void Core::Network::UDP::Manager::__fireEvent(Core::Event::Event* event, Core::Network::UDP::SocketStream* socket) const {
  Core::Network::UDP::EventArgs::SocketStreamArgs* ssargs = Core::Network::UDP::EventArgs::SocketStreamArgs::getFromPool(socket);
  Core::Event::Manager::get().fireEventSync(event, ssargs);
}

void Core::Network::UDP::Manager::__fireEvent(Core::Event::Event* event, Core::Network::UDP::SocketServer* socket) const {
  Core::Network::UDP::EventArgs::SocketServerArgs* ssargs = Core::Network::UDP::EventArgs::SocketServerArgs::getFromPool(socket);
  Core::Event::Manager::get().fireEventSync(event, ssargs);
}

void Core::Network::UDP::Manager::__fireEvent(Core::Event::Event* event, Core::Network::UDP::SocketClient* socket) const {
  Core::Network::UDP::EventArgs::SocketClientArgs* scargs = Core::Network::UDP::EventArgs::SocketClientArgs::getFromPool(socket);
  Core::Event::Manager::get().fireEventSync(event, scargs);
}

/**
 *  Server
 */

Core::Network::UDP::Manager::Server::Server(uint16_t port, Core::Network::UDP::SocketServer* server, const std::set<uint32_t>& accept, const std::set<uint32_t>& blacklist):
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
  eventManager.registerEvent(this->events.onNewClient);
  eventManager.registerEvent(this->events.onReceivedData);
  eventManager.registerEvent(this->events.onClientClosed);
  eventManager.registerEvent(this->events.onClosed);
}

Core::Network::UDP::Manager::Server::~Server(void) {
  Core::Event::Manager& eventManager = Core::Event::Manager::get();
  eventManager.unregisterEvent(this->events.onNewClient);
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClientClosed);
  eventManager.unregisterEvent(this->events.onClosed);
}

/**
 *  Client
 */

Core::Network::UDP::Manager::Client::Client(const std::string& hostname, uint16_t port, Core::Network::UDP::SocketStream* socket):
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

Core::Network::UDP::Manager::Client::~Client(void) {
  Core::Event::Manager& eventManager = Core::Event::Manager::get();
  eventManager.unregisterEvent(this->events.onReceivedData);
  eventManager.unregisterEvent(this->events.onClosed);
}

/**
 *  SocketStreamArgs
 */

Core::Network::UDP::EventArgs::SocketStreamArgs::SocketStreamArgs(void):
  socket(nullptr)
{}

void Core::Network::UDP::EventArgs::SocketStreamArgs::reinit(void) {
  this->socket = nullptr;
}

void Core::Network::UDP::EventArgs::SocketStreamArgs::init(Core::Network::UDP::SocketStream* socket) {
  this->socket = socket;
}

void Core::Network::UDP::EventArgs::SocketStreamArgs::cleanup(void) {
  Core::Network::UDP::EventArgs::SocketStreamArgs::returnToPool(this);
}

/**
 *  SocketServerArgs
 */

Core::Network::UDP::EventArgs::SocketServerArgs::SocketServerArgs(void):
  socket(nullptr)
{}

void Core::Network::UDP::EventArgs::SocketServerArgs::reinit(void) {
  this->socket = nullptr;
}

void Core::Network::UDP::EventArgs::SocketServerArgs::init(Core::Network::UDP::SocketServer* socket) {
  this->socket = socket;
}

void Core::Network::UDP::EventArgs::SocketServerArgs::cleanup(void) {
  Core::Network::UDP::EventArgs::SocketStreamArgs::returnToPool(this);
}

/**
 *  SocketClientArgs
 */

Core::Network::UDP::EventArgs::SocketClientArgs::SocketClientArgs(void):
  socket(nullptr)
{}

void Core::Network::UDP::EventArgs::SocketClientArgs::reinit(void) {
  this->socket = nullptr;
}

void Core::Network::UDP::EventArgs::SocketClientArgs::init(Core::Network::UDP::SocketClient* socket) {
  this->socket = socket;
}

void Core::Network::UDP::EventArgs::SocketClientArgs::cleanup(void) {
  Core::Network::UDP::EventArgs::SocketStreamArgs::returnToPool(this);
}
