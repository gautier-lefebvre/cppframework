#include <iostream>
#include <unistd.h>

#include "Library/Collection/ByteArray.hpp"
#include "Library/Tool/Converter.hpp"
#include "Library/Tool/Logger.hpp"
#include "Library/Tool/Signal.hh"
#include "Core/System.hh"
#include "Core/Exception.hh"
#include "Core/Worker/Manager.hh"
#include "Core/Network/Manager.hh"
#include "Core/Network/HTTP/Client.hh"
#include "Core/Event/Manager.hh"

static void tcpServer(fwk::Core::System* system, uint16_t port) {
  int i = 0;

  try {
    const fwk::Core::Network::TCP::Manager::Server& server = fwk::Core::Network::Manager::get().getTCP().bind(port);

    // on accept new socket callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onAccept, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("New client connected");
    }, &i);

    // on received data callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onReceivedData, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    // on client closed callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onClientClosed, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Client closed");
    }, &i);

    // on server closed callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onClosed, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Server closed");
    }, &i);

    system->run();
  } catch (const fwk::Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void tcpClient(fwk::Core::System* system, const std::string& hostname, uint16_t port) {
  try {
    int i = 0;

    const fwk::Core::Network::TCP::Manager::Client& client = fwk::Core::Network::Manager::get().getTCP().connect(hostname, port);

    fwk::Core::Event::Manager::get().subscribeToEvent(client.events.onReceivedData, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    fwk::Core::Event::Manager::get().subscribeToEvent(client.events.onClosed, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Connection closed");
    }, &i);
    
    fwk::Core::Network::Manager::get().getTCP().push(client.socket, (void*)"Hello", 5);

    system->run();

  } catch (const fwk::Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void udpServer(fwk::Core::System* system, uint16_t port) {
  int i = 0;

  try {
    const fwk::Core::Network::UDP::Manager::Server& server = fwk::Core::Network::Manager::get().getUDP().bind(port);

    // on accept new socket callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onNewClient, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("New client connected");
    }, &i);

    // on received data callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onReceivedData, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    // on client closed callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onClientClosed, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Client closed");
    }, &i);

    // on server closed callback
    fwk::Core::Event::Manager::get().subscribeToEvent(server.events.onClosed, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Server closed");
    }, &i);

    system->run();
  } catch (const fwk::Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void udpClient(fwk::Core::System* system, const std::string& hostname, uint16_t port) {
  try {
    int i = 0;

    const fwk::Core::Network::UDP::Manager::Client& client = fwk::Core::Network::Manager::get().getUDP().connect(hostname, port);

    fwk::Core::Event::Manager::get().subscribeToEvent(client.events.onReceivedData, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Received data");
    }, &i);

    fwk::Core::Event::Manager::get().subscribeToEvent(client.events.onClosed, [] (const fwk::Core::Event::IEventArgs *) {
      INFO("Connection closed");
    }, &i);
    
    fwk::Core::Network::Manager::get().getUDP().push(client.socket, (void*)"Hello", 5);

    system->run();

  } catch (const fwk::Core::Exception& e) {
    CRITICAL(e.what());
  }
}

static void http(fwk::Core::System* system) {
  fwk::Core::Network::HTTP::Request* request;
  fwk::Core::Network::HTTP::Connection* connection = fwk::Core::Network::HTTP::Client::get().initConnection("jsonplaceholder.typicode.com", 80, fwk::Core::Network::HTTP::Protocol::HTTP, true);

  request = fwk::Core::Network::HTTP::Request::getFromPool();
  request->init();
  request->method = "GET";
  request->url = "/posts";
  request->success = [] (const fwk::Core::Network::HTTP::Response* response) -> void {
    INFO(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };
  request->error = [] (const fwk::Core::Network::HTTP::Response* response) -> void {
    WARNING(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };

  fwk::Core::Network::HTTP::Client::get().sendRequest(connection, request);

  request = fwk::Core::Network::HTTP::Request::getFromPool();
  request->init();
  request->method = "GET";
  request->url = "/posts";
  request->success = [] (const fwk::Core::Network::HTTP::Response* response) -> void {
    INFO(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };
  request->error = [] (const fwk::Core::Network::HTTP::Response* response) -> void {
    WARNING(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };

  fwk::Core::Network::HTTP::Client::get().sendRequest(connection, request);

  request = fwk::Core::Network::HTTP::Request::getFromPool();
  request->init();
  request->method = "GET";
  request->url = "/posts";
  request->success = [] (const fwk::Core::Network::HTTP::Response* response) -> void {
    INFO(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };
  request->error = [] (const fwk::Core::Network::HTTP::Response* response) -> void {
    WARNING(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };

  fwk::Core::Network::HTTP::Client::get().sendRequest(connection, request);

  system->run();
}

int main(int ac, char ** av) {
  if ((ac != 2 && ac != 3 && ac != 4) || (std::string(av[1]) != "http" && std::string(av[1]) != "tcp" && std::string(av[1]) != "udp" && std::string(av[1]) != "delayed" && std::string(av[1]) != "periodic" && std::string(av[1]) != "simple")) {
    std::cerr << "usage: " << av[0] << " \"http\"|\"tcp\"|\"udp\" HOSTNAME PORT || " << av[0] << " PORT" << std::endl;
    return -1;
  }

  fwk::Core::System* system = new fwk::Core::System();
  fwk::Signal::get().setCallback(fwk::Signal::Type::INT, [&] (void) -> bool {
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
  } else if (protocol == "http") {
    system->initHTTP("test useragent");
    http(system);
  } else if (protocol == "delayed") {
    system->initWorkerThreads(1, true);
    fwk::Core::Worker::Manager::get().addDelayedTask(fwk::Core::SimpleTask::getFromPool([] (void) {
      INFO("SimpleTask working");
    }), std::chrono::seconds(2));
    system->run();
  } else if (protocol == "periodic") {
    system->initWorkerThreads(1, true);
    fwk::Core::Worker::Manager::get().addPeriodicTask([] (void) {
      INFO("Hello");
    }, nullptr, std::chrono::seconds(5), true);
    system->run();
  } else if (protocol == "simple") {
    fwk::Core::Worker::Manager::get().addSimpleTask([] (void) {
      INFO("SimpleTask :)");
    });
    system->run();
  } else {
    std::cerr << "unknown protocol" << std::endl;
  }

  delete system;
  return 0;
}
