#include  "Core/Network/TCP/Manager.hh"
#include  "Core/Event/Manager.hh"

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
  Core::Network::TCP::SocketStream::returnToPool(this->socket);
  Core::Network::TCP::EventArgs::SocketStreamArgs::returnToPool(this);
}