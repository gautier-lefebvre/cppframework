#include <iostream>
#include <unistd.h>

#include "Library/Collection/ByteArray.hpp"
#include "Library/Tool/Converter.hpp"
#include "Library/Tool/Logger.hpp"
#include "Library/Tool/Signal.hh"
#include "Core/System.hh"
#include "Core/Network/Manager.hh"
#include "Core/Event/Manager.hh"

int main(int ac, char ** av) {
  if (ac != 2) {
    std::cerr << "need port" << std::endl;
    return -1;
  }

  int i = 0;

  uint16_t port = StringToUInt16(av[1]);

  Signal& signal = Signal::get();
  Core::System* system = new Core::System();
  signal.setCallback(Signal::Type::INT, [&] (void) {
    INFO("Caught SIGINT, exiting.");
    system->end();
  });

  system->initTCP();

  const Core::Network::TCP::Manager::Server& server = Core::Network::Manager::get().getTCP().bind(port);


  Core::Event::Manager::get().subscribeToEvent(server.events.onReceivedData, [] (const Core::Event::IEventArgs * args) {
    const Core::Network::TCP::EventArgs::SocketStreamArgs* ssargs = reinterpret_cast<const Core::Network::TCP::EventArgs::SocketStreamArgs*>(args);

    ByteArray* bytearray = ByteArray::getFromPool();
    ssargs->socket->extractData([] (const ByteArray& data) -> size_t {
      return data.getSize() > 0 ? data.getSize() : std::string::npos;
    }, bytearray);

    INFO("Received: ");
    INFO(*bytearray);

    ByteArray::returnToPool(bytearray);
  }, &i);

  system->run();

  delete system;
  return 0;
}
