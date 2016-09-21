#include  "Core/Network/Udp/UdpServer.hh"

using namespace fwk;

UdpServer::UdpServer(uint16_t port, UdpSocketServer* server):
  Lockable(),
  port(port),
  server(server),
  clients(),
  accept(),
  blacklist(),
  active(false),
  events()
{}

UdpServer::~UdpServer(void) {}
