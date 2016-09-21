#include  <algorithm>

#include  "Library/ThirdParty/cppformat/format.hh"
#include  "Library/Tool/Converter.hpp"
#include  "Library/Tool/Macro.hh"
#include  "Core/Network/Udp/UdpManager.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

UdpManager::UdpManager(NotifiableThread& input, NotifiableThread& output):
  _servers(),
  _clients(),
  _input(input),
  _output(output)
{}

UdpManager::~UdpManager(void) {}

void UdpManager::clear(void) {

  // close every server
  {
    SCOPELOCK(&(this->_servers));
    for (auto server_it = this->_servers.begin(); server_it != this->_servers.end() ; ++server_it) {
      for (auto& client : (*server_it).clients) {
        // fire onClientClosed event
        (*server_it).events.onClientClosed.fireSync(client);
        // close client
        UdpSocketClient::returnToPool(client);
      }
      // fire onClosed event
      (*server_it).events.onClosed.fireSync((*server_it).server);
      // close server
      UdpSocketServer::returnToPool((*server_it).server);
      // remove server
      server_it = this->_servers.erase(server_it);
    }
  }

  // close every connection
  {
    SCOPELOCK(&(this->_clients));
    for (auto client_it = this->_clients.begin(); client_it != this->_clients.end() ; ++client_it) {
      // fire onClosed event
      (*client_it).events.onClosed.fireSync((*client_it).socket);
      // close connection
      UdpSocketStream::returnToPool((*client_it).socket);
      // remove connection
      client_it = this->_clients.erase(client_it);
    }
  }
}

UdpServer& UdpManager::createServer(uint16_t port) {
  UdpSocketServer* socket = UdpSocketServer::getFromPool();

  try {
    socket->init();
    socket->socket();

    UdpServer* server = nullptr;

    {
      SCOPELOCK(&(this->_servers));

      if (std::find_if(this->_servers.begin(), this->_servers.end(), [=] (const UdpServer& s) -> bool { return s.port == port; }) != this->_servers.end()) {
        throw NetworkException(fmt::format("The port {0} was already associated to a server", port));
      }

      this->_servers.emplace_back(port, socket);
      server = &(this->_servers.back());
    }

    return *server;
  } catch (const NetworkException&) {
    UdpSocketServer::returnToPool(socket);
    throw;
  }
}

void UdpManager::run(const UdpServer& server) {
  {
    SCOPELOCK(&(this->_servers));
    auto serverIt = std::find_if(this->_servers.begin(), this->_servers.end(), [&] (const UdpServer& s) -> bool { return s.port == server.port; });

    if (serverIt != this->_servers.end()) {
      try {
        UdpServer& s = (*serverIt);
        s.server->bind(s.port);
        s.active = true;
        INFO(fmt::format("UDP: listening on port {0}", s.port));
      } catch (const std::exception&) {
        UdpSocketServer::returnToPool((*serverIt).server);
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


void UdpManager::close(uint16_t port) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto server_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const UdpServer& elem) -> bool { return elem.port == port; });

  // if found
  if (server_it != this->_servers.end()) {
    for (auto& client : (*server_it).clients) {
      // fire onClientClosed event
      (*server_it).events.onClientClosed.fireSync(client);
      // send client back to pool
      UdpSocketClient::returnToPool(client);
    }

    // fire server close event
    (*server_it).events.onClosed.fireSync((*server_it).server);
    // close server
    UdpSocketServer::returnToPool((*server_it).server);

    // remove server
    this->_servers.erase(server_it);
    return;
  }
}

void UdpManager::close(const UdpServer& server) {
  this->close(server.port);
}

void UdpManager::blacklist(uint16_t port, uint32_t addr) {
  SCOPELOCK(&(this->_servers));

  // find the server
  auto server_it = std::find_if(this->_servers.begin(), this->_servers.end(),
    [&] (const UdpServer& elem) -> bool { return elem.port == port; });

  // if found
  if (server_it != this->_servers.end()) {
    (*server_it).blacklist.insert(addr);
    return;
  }
}

UdpClient& UdpManager::createClient(const std::string& hostname, uint16_t port) {
  UdpSocketStream* socket = UdpSocketStream::getFromPool();

  try {
    socket->socket();
    socket->init(hostname, port);

    UdpClient* client = nullptr;

    {
      SCOPELOCK(&(this->_clients));

      if (std::find_if(this->_clients.begin(), this->_clients.end(), [=] (const UdpClient& c) -> bool { return c.hostname == hostname && c.port == port; }) != this->_clients.end()) {
        throw NetworkException(fmt::format("A client to {0}:{1} already exists", hostname, port));
      }

      this->_clients.emplace_back(hostname, port, socket);
      client = &(this->_clients.back());
    }

    return *client;
  } catch (const NetworkException&) {
    UdpSocketStream::returnToPool(socket);
    throw;
  }
}

void UdpManager::run(const UdpClient& client) {
  {
    SCOPELOCK(&(this->_clients));
    auto clientIt = std::find_if(this->_clients.begin(), this->_clients.end(), [&] (const UdpClient& c) -> bool { return c.hostname == client.hostname && c.port == client.port; });

    if (clientIt != this->_clients.end()) {
      try {
        UdpClient& c = (*clientIt);
        c.active = true;
        INFO(fmt::format("UDP: prepared connection to {0}:{1}", c.hostname, c.port));

      } catch (const std::exception&) {
        UdpSocketStream::returnToPool((*clientIt).socket);
        this->_clients.erase(clientIt);
        throw;
      }
    } else {
      throw NetworkException("This client was never created");
    }
  }
}

void UdpManager::close(const std::string& hostname, uint16_t port) {
  SCOPELOCK(&(this->_clients));

  // find the connection
  auto client_it = std::find_if(this->_clients.begin(), this->_clients.end(),
    [&] (const UdpClient& elem) -> bool { return elem.hostname == hostname && elem.port == port; });

  // if found
  if (client_it != this->_clients.end()) {
    // fire close event
    (*client_it).events.onClosed.fireSync((*client_it).socket);
    // close socket
    UdpSocketStream::returnToPool((*client_it).socket);

    // remove connection
    this->_clients.erase(client_it);
    return;
  }
}

void UdpManager::close(const UdpClient& connection) {
  this->close(connection.hostname, connection.port);
}

void UdpManager::push(AUdpSocketIO* socket, const void* data, size_t size) {
  if (socket != nullptr) {
  	ByteArray* datagram = ByteArray::getFromPool(data, size);

    try {
      socket->push(datagram);

      {
        SCOPELOCK(&(this->_output.condition));
        this->_output.condition.notify();
      }
    } catch (const NetworkException& e) {
    	ByteArray::returnToPool(datagram);

      // find server
      {
        SCOPELOCK(&(this->_servers));
        for (auto& server : this->_servers) {
          auto client_it = std::find(server.clients.begin(), server.clients.end(), socket);
          if (client_it != server.clients.end()) {
            this->__onIOException(server.events.onClientClosed, reinterpret_cast<UdpSocketClient*>(socket), e.what());
            server.clients.erase(client_it);
            return;
          }
        }
      }

      // find connection
      {
        SCOPELOCK(&(this->_clients));
        auto client_it = std::find_if(this->_clients.begin(), this->_clients.end(),
          [&] (const UdpClient& elem) -> bool { return elem.socket == socket; });

        if (client_it != this->_clients.end()) {
          this->__onIOException((*client_it).events.onClosed, reinterpret_cast<UdpSocketStream*>(socket), e.what());
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

void UdpManager::push(AUdpSocketIO* socket, const ByteArray* bytearray) {
  if (bytearray != nullptr) {
    this->push(socket, bytearray->getBytes(), bytearray->getSize());
  }
}

void UdpManager::fillSetRead(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.active) {
        server.server->addToSet(fdset, fdmax);
        nb++;
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

void UdpManager::fillSetWrite(fd_set& fdset, int& fdmax, uint32_t& nb) {
  {
    SCOPELOCK(&(this->_servers));
    // for each server -> add to set if at least one client has data to send
    for (auto& server : this->_servers) {
      if (server.active) {
        for (auto& client : server.clients) {
          if (client->hasDataToSend()) {
            server.server->addToSet(fdset, fdmax);
            nb++;
            break;
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

void UdpManager::send(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    for (auto& server : this->_servers) {
      if (server.active && server.server->isset(set)) {
        for (auto client_it = server.clients.begin() ; client_it != server.clients.end() ; ++client_it) {
          if ((*client_it)->hasDataToSend()) {
            try {
              server.server->sendto(*client_it);
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
          (*client_it).socket->sendto();
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

void UdpManager::recv(fd_set& set) {
  {
    SCOPELOCK(&(this->_servers));
    sockaddr_in addr;

    for (auto& server : this->_servers) {
      if (server.active && server.server->isset(set)) {
        memset(&addr, 0, sizeof(sockaddr_in)); // reinit the addr in order not to remove a valid client because of old data
        ByteArray* datagram = nullptr;
        bool success = true; // true -> recvfrom did not except / false -> recvfrom did except

        try {
          datagram = server.server->recvfrom(addr);
        } catch (const NetworkException& e) {
          WARNING(e.what());
          continue;
        }

        // find client
        auto client_it = std::find_if(server.clients.begin(), server.clients.end(),
          [&] (const UdpSocketClient* socket) -> bool { return *socket == addr; });

        if (success) {
          try {
            // if client found -> add datagram
            // if client not found -> check if client not blacklisted / accepted
            //                        add client + add datagram
            if (client_it != server.clients.end()) {
              (*client_it)->received(datagram);

              // fire data received event
              server.events.onReceivedData.fireSync(*client_it);
            } else {
              uint32_t remote_ip = static_cast<uint32_t>(addr.sin_addr.s_addr);

              // if accept list is not empty and remote ip not in it
              if (!(server.accept.empty()) && std::find(server.accept.begin(), server.accept.end(), remote_ip) == server.accept.end()) {
                throw NetworkException("Unauthorized IP attempted to send data (UDP)");
              }

              // if blacklisted ip
              if (!(server.blacklist.empty()) && std::find(server.blacklist.begin(), server.blacklist.end(), remote_ip) != server.blacklist.end()) {
                throw NetworkException("Blacklisted IP attempted to send data (UDP)");
              }

              UdpSocketClient* client = UdpSocketClient::getFromPool(addr);

              try {
                client->received(datagram);
              } catch (const NetworkException&) {
                WARNING("Something went wrong, a single datagram could not overflow the limit");
                UdpSocketClient::returnToPool(client);
                throw;
              }

              server.clients.push_back(client);
              // fire new client event
              server.events.onNewClient.fireSync(client);
              // fire data received event
              server.events.onReceivedData.fireSync(client);
            }
          } catch (const NetworkException& e) {
            ByteArray::returnToPool(datagram);
            WARNING(e.what());
          }
        } else {
          // if recvfrom excepted and client in list -> remove client
          if (client_it != server.clients.end()) {
            // fire onClientClosed event
            server.events.onClientClosed.fireSync(*client_it);
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
      if ((*client_it).active && (*client_it).socket->isset(set)) {
        try {
          // receive data
          (*client_it).socket->recvfrom();
          // fire onReceivedData event
          (*client_it).events.onReceivedData.fireSync((*client_it).socket);
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

void UdpManager::__onIOException(EventHandle<UdpSocketClient*>& event, UdpSocketClient* socket, const std::string&) {
  // fire closed event
  event.fireSync(socket);
  // close socket
  UdpSocketClient::returnToPool(socket);
}

void UdpManager::__onIOException(EventHandle<UdpSocketStream*>& event, UdpSocketStream* socket, const std::string&) {
  // fire closed event
  event.fireSync(socket);
  // close socket
  UdpSocketStream::returnToPool(socket);
}
