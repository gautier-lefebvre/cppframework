#include  "Core/Network/Tcp/TcpClient.hh"

using namespace fwk;

TcpClient::TcpClient(const std::string& hostname, uint16_t port, TcpSocketStream* socket):
  Lockable(),
  hostname(hostname),
  port(port),
  socket(socket),
  active(false),
  events()
{}

TcpClient::~TcpClient(void) {}
