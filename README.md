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

## documentation

See the wiki section of the project.

## modules

The framework is based on modules, some of which are optional. You can decide to have a TCP/IP server/client manager (see below), a UDP server/client manager, etc.

### events

Any object you create can declare some events. The object providing the events must register them to the Event Manager (`Core::Event::Manager`), and unregister them before destroying the object.

Another object can subscribe to a registered event, and provide a callback method which will be called whenever the event is fired.

Be careful though, you must NEVER fire an asynchronous event upon destruction of your object, otherwise said event will be executed after your object is destroyed, making it impossible to unregister the event.

You must always unregister your events before destroying them, to prevent a fired asynchronous event to be executed after the event was destroyed. It can either end up with a segfault if you deleted the event, undefined behaviour if you didn't. If you at least returned the event to the pool, it will be unregistered instead of being executed.

Events can now be fired synchronously, but you'd better be sure of what you are doing, it stops the execution of the calling thread to execute the callback, so keep the operations to a minimum. If the operation is heavy you can always prepare a new task in the event callback, add it to the task queue and execute the operation in the task callback.

### networking

#### tcp/ip

The TCP/IP module offers you the possibility to bind sockets and act as a server, or connect to remote TCP servers and act as a client, at the same time. It runs on 2 threads, 1 for input and 1 for output.

TCP servers offer 4 events:
- a client connected.
- a client disconnected.
- a client sent data.
- the server closed.

TCP clients offer 2 events:
- the server sent data.
- the client closed.

All these events are synchronous.

#### udp/ip

The UDP/IP module works in the same way as the TCP/IP one. The main difference is that the event for socket closing is not accurate, due to the fact that the UDP protocol is not connected.

#### http

The HTTP module is a HTTP client. It works this way: each remote server runs on a different thread, as you can't send a new request before completely reading the response to the last request you sent. Whenever you want to send a HTTP request, the manager will check if you already have a connection to this url (defined by a path and a port), and create a new thread dynamically. You don't have to initialize anything, but you have the choice.

When sending a HTTP request, you can provide a callback method which will be called when the response to this particular request is read.

You can send synchronous HTTP requests. The thread is woken if the response is read or if the system is being shut down.

This uses cURL.

### workers

Every task is put in a shared queue (FIFO collection).
They are then executed by worker threads. There must be at least 1 worker thread.

When the framework is being cleaned, every task still in the task queue is cleared.
Most tasks can provide a **cleanup** function which can be used to clean any resource allocated for the normal execution callback.

#### tasks

##### SimpleTask

A simple task is simply an action put at the end of the task queue. You must provide a function with the following prototype: `std::function<void (void)>`.
You can also provide a cleanup function with the same prototype (see [workers](#workers)).

##### EventTask

This is the task to create when firing an event. There is a shortcut through the event itself, but it creates the EventTask object and adds it to the task queue.

##### HTTPTask

This is the type of task created by the HTTP client module when receiving a response to a HTTP request.

##### PeriodicTask

A periodic task is a task which is executed at regular interval. You set a callback in the form of `std::function<void (void)>`, a cleanup function called when the periodic task is canceled, and a duration as interval.

#### delayed tasks

You can choose to execute a task after a delay. This requires an extra thread, which puts the delayed tasks into the tasks queue after the delay is completed.

## factory

The framework tries to be the most efficient possible. With that in mind, it tries never to allocate anything dynamically, which means that most of the framework's objects are pooled.

Any pooled object can be retrieved from its pool with a `Foo::Bar::getFromPool(args...)` et returned to their pool with a `Foo::Bar::returnToPool(object)`.

## threads

Based on the modules you activate and the number of workers you choose, you will have a certain amount of threads:
- TCP module: 2 threads.
- UDP module: 2 threads (if both TCP and UDP is activated, they will use the same threads).
- HTTP module: 1 thread for each remote server.
- Delayed tasks: 1 thread.
- Worker thread: at least 1 thread.

## third party

Included third party libraries and functions:
- [cppformat](http://cppformat.github.io/latest/)
