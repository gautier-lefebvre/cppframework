#include <iostream>
#include <unistd.h>

#include "Library/Collection/ByteArray.hpp"
#include "Library/Tool/Converter.hpp"
#include "Library/Tool/Logger.hpp"
#include "Library/Tool/Signal.hh"
#include "Core/System.hh"
#include "Core/Exception.hh"
#include "Core/Network/Manager.hh"
#include "Core/Event/Manager.hh"

static void tcpServer(Core::System* system, uint16_t port) {
  int i = 0;

  try {
    const Core::Network::TCP::Manager::Server& server = Core::Network::Manager::get().getTCP().bind(port);

    // on accept new socket callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onAccept, [] (const Core::Event::IEventArgs *) {
      INFO("New client connected");
    }, &i);

    // on received data callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onReceivedData, [] (const Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    // on client closed callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onClientClosed, [] (const Core::Event::IEventArgs *) {
      INFO("Client closed");
    }, &i);

    // on server closed callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onClosed, [] (const Core::Event::IEventArgs *) {
      INFO("Server closed");
    }, &i);

    system->run();
  } catch (const Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void tcpClient(Core::System* system, const std::string& hostname, uint16_t port) {
  try {
    int i = 0;

    const Core::Network::TCP::Manager::Client& client = Core::Network::Manager::get().getTCP().connect(hostname, port);

    Core::Event::Manager::get().subscribeToEvent(client.events.onReceivedData, [] (const Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    Core::Event::Manager::get().subscribeToEvent(client.events.onClosed, [] (const Core::Event::IEventArgs *) {
      INFO("Connection closed");
    }, &i);
    
    Core::Network::Manager::get().getTCP().push(client.socket, (void*)"Hello", 5);

    system->run();

  } catch (const Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void udpServer(Core::System* system, uint16_t port) {
  int i = 0;

  try {
    const Core::Network::UDP::Manager::Server& server = Core::Network::Manager::get().getUDP().bind(port);

    // on accept new socket callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onNewClient, [] (const Core::Event::IEventArgs *) {
      INFO("New client connected");
    }, &i);

    // on received data callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onReceivedData, [] (const Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    // on client closed callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onClientClosed, [] (const Core::Event::IEventArgs *) {
      INFO("Client closed");
    }, &i);

    // on server closed callback
    Core::Event::Manager::get().subscribeToEvent(server.events.onClosed, [] (const Core::Event::IEventArgs *) {
      INFO("Server closed");
    }, &i);

    system->run();
  } catch (const Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void udpClient(Core::System* system, const std::string& hostname, uint16_t port) {
  try {
    int i = 0;

    const Core::Network::UDP::Manager::Client& client = Core::Network::Manager::get().getUDP().connect(hostname, port);

    Core::Event::Manager::get().subscribeToEvent(client.events.onReceivedData, [] (const Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    Core::Event::Manager::get().subscribeToEvent(client.events.onClosed, [] (const Core::Event::IEventArgs *) {
      INFO("Connection closed");
    }, &i);
    
    Core::Network::Manager::get().getUDP().push(client.socket, (void*)"Hello", 5);

    system->run();

  } catch (const Core::Exception& e) {
    CRITICAL(e.what());
  }
}

int main(int ac, char ** av) {
  if ((ac != 3 && ac != 4) || (std::string(av[1]) != "tcp" && std::string(av[1]) != "udp")) {
    std::cerr << "usage: " << av[0] << " \"tcp\"|\"udp\" HOSTNAME PORT || " << av[0] << " PORT" << std::endl;
    return -1;
  }

  Core::System* system = new Core::System();
  Signal::get().setCallback(Signal::Type::INT, [&] (void) -> bool {
    INFO("Caught SIGINT, exiting.");
    system->end();
    return false;
  });

  std::string protocol = av[1];

  if (protocol == "udp") {
    system->initUDP();
    if (ac == 3) {
      // udp server
      udpServer(system, StringToUInt16(av[2]));
    } else if (ac == 4) {
      // udp client
      udpClient(system, av[2], StringToUInt16(av[3]));
    } else {
      std::cerr << "invalid nb of arguments" << std::endl;
    }
  } else if (protocol == "tcp") {
    system->initTCP();
    if (ac == 3) {
      // tcp server
      tcpServer(system, StringToUInt16(av[2]));
    } else if (ac == 4) {
      // tcp client
      tcpClient(system, av[2], StringToUInt16(av[3]));
    } else {
      std::cerr << "invalid nb of arguments" << std::endl;
    }
  } else {
    std::cerr << "unknown protocol" << std::endl;
  }

  delete system;
  return 0;
}
