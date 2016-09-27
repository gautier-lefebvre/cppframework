#include  "Core/Network/Udp/UdpClient.hh"

using namespace fwk;

UdpClient::UdpClient(const std::string& hostname, uint16_t port, UdpSocketStream* socket):
  hostname(hostname),
  port(port),
  socket(socket),
  active(false),
  events()
{}

UdpClient::~UdpClient(void) {}

