#include  "Core/Network/Tcp/TcpServer.hh"

using namespace fwk;

TcpServer::TcpServer(uint16_t port, TcpSocket* server):
    Lockable(),
    port(port),
    server(server),
    clients(),
    accept(),
    blacklist(),
    active(false),
    events()
{}

TcpServer::~TcpServer(void) {}
