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

// static void tcpServer(Core::System* system, uint16_t port) {
//   int i = 0;

//   try {
//     const Core::Network::TCP::Manager::Server& server = Core::Network::Manager::get().getTCP().bind(port);

//     // on accept new socket callback
//     Core::Event::Manager::get().subscribeToEvent(server.events.onAccept, [] (const Core::Event::IEventArgs *) {
//       INFO("New client connected");
//     }, &i);

//     // on received data callback
//     Core::Event::Manager::get().subscribeToEvent(server.events.onReceivedData, [] (const Core::Event::IEventArgs *) {
//       INFO("Received data");
//     }, &i);

//     // on client closed callback
//     Core::Event::Manager::get().subscribeToEvent(server.events.onClientClosed, [] (const Core::Event::IEventArgs *) {
//       INFO("Client closed");
//     }, &i);

//     // on server closed callback
//     Core::Event::Manager::get().subscribeToEvent(server.events.onClosed, [] (const Core::Event::IEventArgs *) {
//       INFO("Server closed");
//     }, &i);

//     system->run();
//   } catch (const Core::Exception& e) {
//     CRITICAL(e.what());
//   }
// }

// static void tcpClient(Core::System* system, const std::string& hostname, uint16_t port) {
//   try {
//     int i = 0;

//     const Core::Network::TCP::Manager::Client& client = Core::Network::Manager::get().getTCP().connect(hostname, port);

//     Core::Event::Manager::get().subscribeToEvent(client.events.onReceivedData, [] (const Core::Event::IEventArgs *) {
//       INFO("Received data");
//     }, &i);

//     Core::Event::Manager::get().subscribeToEvent(client.events.onClosed, [] (const Core::Event::IEventArgs *) {
//       INFO("Connection closed");
//     }, &i);
    
//     Core::Network::Manager::get().getTCP().push(client.socket, (void*)"Hello", 5);

//     system->run();

//   } catch (const Core::Exception& e) {
//     CRITICAL(e.what());
//   }
// }

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
  if (ac != 2 && ac != 3) {
    std::cerr << "usage: " << av[0] << " HOSTNAME PORT || " << av[0] << " PORT" << std::endl;
    return -1;
  }

  Core::System* system = new Core::System();
  Signal::get().setCallback(Signal::Type::INT, [&] (void) -> bool {
    INFO("Caught SIGINT, exiting.");
    system->end();
    return false;
  });

  system->initUDP();

  if (ac == 2) {
    // tcp server
    udpServer(system, StringToUInt16(av[1]));
  } else {
    // tcp client
    udpClient(system, av[1], StringToUInt16(av[2]));
  }

  delete system;
  return 0;
}
