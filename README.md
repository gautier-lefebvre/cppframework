# cppframework

A C++ framework which can be used in an event-handling architecture (server/client, etc).

It is written is C++11.

It is still in development. Information below is for now only the requisites.

## operating system

For now I'm only focusing on Debian-based environments, but Windows will also be supported.

## dependencies

- cURL
- pthread
- ssl

## modules

The framework is based on modules, some of which are optional. You can decide to have a TCP/IP server/client manager (see below), a UDP server/client manager, etc.

### networking

#### tcp/ip

The TCP/IP module offers you the possibility to bind sockets and act as a server, or connect to distant servers and act as a client, at the same time. It runs on 2 threads, 1 for input and 1 for output.

When you bind a socket or connect to one, you can attach a callback which will be called whenever data is read from it, or in the case of the bound socket, whenever data is read from a distant socket which connected to yours.

#### udp/ip

The UDP/IP module works in the same way as the TCP/IP one, except that the callback method is called whenever a datagram is read.

#### http

The HTTP module is a HTTP client. It works this way: each remote server runs on a different thread, as you can't send a new request before completely reading the response to the last request you sent. Whenever you want to send a HTTP request, the manager will check if you already have a connection to this url (defined by a path and a port), and create a new thread dynamically. You don't have to initialize anything, but you have the choice.

When sending a HTTP request, you can provide a callback method which will be called when the response to this particular request is read.

This uses cURL.

### workers

Every event or action (data read from a socket, response received from a HTTP request) is put in a shared queue (FIFO collection).

They are then executed by worker threads. There must be at least 1 worker thread.

### events

Any object you create can declare some events. The object providing the events must register them to the Event Manager (`Core::Event::Manager`), and unregister them before destroying the object.

Another object can subscribe to a registered event, and provide a callback method which will be called whenever the event is fired.

Be careful though, you should NEVER declare an event that would be fired upon destruction of your object, since they are always executed asynchronously, which means after your object would have been destroyed (is that english ?). Anyway, the address of your event would be invalid.

You must always unregister your events before destroying them, to prevent a launched asynchronous event to be executed after the event was destroyed. It can either end up with a segfault if you deleted the event, undefined behaviour if you didn't. If you at least returned the event to the pool, it will be unregistered instead of being executed.

#### delayed events

If you want an event to be executed after a delay, you can use delayed events, which will need an extra thread (1 for all events). You only need to create an event, specify a delay and add it to the worker manager, which will then pass it to the timout events thread. The event will be added to the action queue after the delay is completed.

## factory

The framework tries to be the most efficient possible. With that in mind, it tries never to allocate anything dynamically, which means that most of the framework's objects are pooled.

## threads

Based on the modules you activate and the number of workers you choose, you will have a certain amount of threads:
- TCP module: 2 threads.
- UDP module: 2 threads (if both TCP and UDP is activated, they will use the same threads).
- HTTP module: 1 thread for each remote server.
- Delayed events: 1 thread.
- Worker: at least 1 thread.

## third party

Third party libraries and functions:
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
- [utfcpp](http://sourceforge.net/projects/utfcpp/)
- [reader-writer lock with writer priority](http://code.activestate.com/recipes/577803-reader-writer-lock-with-priority-for-writers/) (from python)
- [SHA-512](http://www.zedwood.com/article/cpp-sha512-function)
- [cppformat](http://cppformat.github.io/latest/)
