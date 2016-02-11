#include <iostream>
#include <unistd.h>

#include "Library/Collection/ByteArray.hpp"
#include "Library/Tool/Converter.hpp"
#include "Library/Tool/Logger.hpp"
#include "Library/Tool/Signal.hh"
#include "Core/System.hh"
#include "Core/Exception.hh"
#include "Core/Worker/WorkerManager.hh"
#include "Core/Network/NetworkManager.hh"
#include "Core/Network/Http/HttpClient.hh"
#include "Core/Event/EventManager.hh"

static void tcpServer(fwk::System* system, uint16_t port) {
  int i = 0;

  try {
    const fwk::TcpManager::Server& server = fwk::NetworkManager::get().getTCP().createServer(port);

    server.events.onAccept->subscribe([] (const fwk::IEventArgs *) {
      INFO("New client connected");
    }, &i);

    server.events.onReceivedData->subscribe([] (const fwk::IEventArgs *) {
      INFO("Received data");
    }, &i);

    server.events.onClientClosed->subscribe([] (const fwk::IEventArgs *) {
      INFO("Client closed");
    }, &i);

    server.events.onClosed->subscribe([] (const fwk::IEventArgs *) {
      INFO("Server closed");
    }, &i);

    fwk::NetworkManager::get().getTCP().run(server);

    system->run();
  } catch (const fwk::CoreException& e) {
    CRITICAL(e.what());
  }
}

static void tcpClient(fwk::System* system, const std::string& hostname, uint16_t port) {
  try {
    int i = 0;

    const fwk::TcpManager::Client& client = fwk::NetworkManager::get().getTCP().createClient(hostname, port);

    client.events.onReceivedData->subscribe([] (const fwk::IEventArgs *) {
      INFO("Received data");
    }, &i);

    client.events.onClosed->subscribe([] (const fwk::IEventArgs *) {
      INFO("Connection closed");
    }, &i);

    fwk::NetworkManager::get().getTCP().run(client);

    fwk::NetworkManager::get().getTCP().push(client.socket, (void*)"Hello", 5);

    system->run();
  } catch (const fwk::CoreException& e) {
    CRITICAL(e.what());
  }
}

static void udpServer(fwk::System* system, uint16_t port) {
  int i = 0;

  try {
    const fwk::UdpManager::Server& server = fwk::NetworkManager::get().getUDP().bind(port);

    // on accept new socket callback
    fwk::EventManager::get().subscribeToEvent(server.events.onNewClient, [] (const fwk::IEventArgs *) {
      INFO("New client connected");
    }, &i);

    // on received data callback
    fwk::EventManager::get().subscribeToEvent(server.events.onReceivedData, [] (const fwk::IEventArgs *) {
      INFO("Received data");
    }, &i);

    // on client closed callback
    fwk::EventManager::get().subscribeToEvent(server.events.onClientClosed, [] (const fwk::IEventArgs *) {
      INFO("Client closed");
    }, &i);

    // on server closed callback
    fwk::EventManager::get().subscribeToEvent(server.events.onClosed, [] (const fwk::IEventArgs *) {
      INFO("Server closed");
    }, &i);

    system->run();
  } catch (const fwk::CoreException& e) {
    CRITICAL(e.what());
  }
}

static void udpClient(fwk::System* system, const std::string& hostname, uint16_t port) {
  try {
    int i = 0;

    const fwk::UdpManager::Client& client = fwk::NetworkManager::get().getUDP().connect(hostname, port);

    fwk::EventManager::get().subscribeToEvent(client.events.onReceivedData, [] (const fwk::IEventArgs *) {
      INFO("Received data");
    }, &i);

    fwk::EventManager::get().subscribeToEvent(client.events.onClosed, [] (const fwk::IEventArgs *) {
      INFO("Connection closed");
    }, &i);
    
    fwk::NetworkManager::get().getUDP().push(client.socket, (void*)"Hello", 5);

    system->run();

  } catch (const fwk::CoreException& e) {
    CRITICAL(e.what());
  }
}

static void http(fwk::System* system) {
  fwk::HttpRequest* request;
  fwk::HttpConnection* connection = fwk::HttpClient::get().initConnection("jsonplaceholder.typicode.com", 80, fwk::HttpProtocol::HTTP, true);

  request = fwk::HttpRequest::getFromPool();
  request->init();
  request->method = "GET";
  request->url = "/posts";
  request->success = [] (const fwk::HttpResponse* response) -> void {
    INFO(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };
  request->error = [] (const fwk::HttpResponse* response) -> void {
    WARNING(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };

  fwk::HttpClient::get().sendRequest(connection, request);

  request = fwk::HttpRequest::getFromPool();
  request->init();
  request->method = "GET";
  request->url = "/posts";
  request->success = [] (const fwk::HttpResponse* response) -> void {
    INFO(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };
  request->error = [] (const fwk::HttpResponse* response) -> void {
    WARNING(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };

  fwk::HttpClient::get().sendRequest(connection, request);

  request = fwk::HttpRequest::getFromPool();
  request->init();
  request->method = "GET";
  request->url = "/posts";
  request->success = [] (const fwk::HttpResponse* response) -> void {
    INFO(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };
  request->error = [] (const fwk::HttpResponse* response) -> void {
    WARNING(fmt::format("Response: {} / Size: {}", response->status, response->body->getSize()));
  };

  fwk::HttpClient::get().sendRequest(connection, request);

  system->run();
}

int main(int ac, char ** av) {
  if ((ac != 2 && ac != 3 && ac != 4) || (std::string(av[1]) != "http" && std::string(av[1]) != "tcp" && std::string(av[1]) != "udp" && std::string(av[1]) != "delayed" && std::string(av[1]) != "periodic" && std::string(av[1]) != "simple")) {
    std::cerr << "usage: " << av[0] << " \"http\"|\"tcp\"|\"udp\" HOSTNAME PORT || " << av[0] << " PORT" << std::endl;
    return -1;
  }

  fwk::System* system = new fwk::System();
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
    fwk::WorkerManager::get().addDelayedTask(fwk::SimpleTask::getFromPool([] (void) {
      INFO("SimpleTask working");
    }), std::chrono::seconds(2));
    system->run();
  } else if (protocol == "periodic") {
    system->initWorkerThreads(1, true);
    fwk::WorkerManager::get().addPeriodicTask([] (void) {
      INFO("Hello");
    }, nullptr, std::chrono::seconds(5), true);
    system->run();
  } else if (protocol == "simple") {
    fwk::WorkerManager::get().addSimpleTask([] (void) {
      INFO("SimpleTask :)");
    });
    system->run();
  } else {
    std::cerr << "unknown protocol" << std::endl;
  }

  delete system;
  return 0;
}
