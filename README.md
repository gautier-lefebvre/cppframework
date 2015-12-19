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

You must always unregister your events before destroying them.

Below, an example:
- MyClass creates a list and 2 events: one which will be fired when a value is inserted in the list, and one which is fired when the list is cleared.
- MyClass registers its events.
- MySubscriber creates an instance of MyClass and subscribes to the event onInsert, providing a callback method.
- MyClass fires the events in methods `clear` and `insert`, passing a struct inheriting from Core::Event::IEventArgs and containing the value inserted for the latter.
- MySubscriber prints the value inserted in the callback method.

(Note: this is deprecated (mostly the pool part is simplified, and you can also fire events synchronously))
```
#include <list>
#include <iostream> // to print

#include "Library/Factory/Pool.hpp" // for BasicPool
#include "Core/Event/Event.hh" // for events
#include "Core/Event/IEventArgs.hh" // for events
#include "Core/Worker/Manager.hh" // to fire events

class MyClass {
	std::list<int> _mylist; // this is just for the example

public:
	// declare some events about the object (here, on the list)
	struct {
		Core::Event::Event onInsert; // on inserting an object in the list
		Core::Event::Event onClear; // on clearing the list
	} events;

	// declare event arguments
	// this is what is sent to a subscriber upon firing an event
	struct onInsertArguments :public Core::Event::IEventArgs {
		// for example, we send the inserted value as argument
		int value;

		// must implement from IEventArgs (before sending back to pool)
		// this implementation is useless ofc
		virtual void reinit(void) { this->value = 0; }

		// since we use a pool, we can declare a method init (you don't have to)
		void init(int v) {
			this->value = v;
		}

		// you should always use a pool for event arguments, but that's up to you
		struct Pool :public Factory::BasicPool<onInsertArguments> {
			const size_t ORIGINAL_SIZE = 10; // number of elements in the pool at start
			const size_t HYDRATE_SIZE = 2; // number of elements constructed when the pool is empty

			void init(void) {
				// create the pool
				this->initPool(MyClass::onInsertArguments::Pool::ORIGINAL_SIZE,
				MyClass::onInsertArguments::Pool::HYDRATE_SIZE,
				"MyClass::onInsertArguments");
			}
		};
	};

public:
	MyClass(void): _mylist(), events() {
		// register events
		// you can do it anywhere, you just need to register them before any object can subscribe to it, and before you can fire them
		Core::Event::Manager::get().register(
			&(this->events.onInsert), // your event
			[] (Core::Event::IEventArgs* ptr) -> void {
				// this is a cleaning function
				// since we used a pool for the arguments, we will send them back to the pool
				// if you don't use a pool, you can just delete it

				// reinterpret the args
				MyClass::onInsertedArguments* args = reinterpret_cast<MyClass::onInsertedArguments*>(ptr);

				// you can assert that this is safe, as long as you always pass this type of argument when firing
				if (args) {
					// send the args back to the pool
					MyClass::onInsertedArguments::Pool::get().remove(args);
				}
			}
		);

		// there are no arguments to onClear, so no cleaning function needed
		Core::Event::Manager::get().register(&(this->events.onClear), nullptr);
	}

	~MyClass(void) {
		// we unregister our events
		Core::Event::Manager::get().unregister(&(this->events.onInsert));
		Core::Event::Manager::get().unregister(&(this->events.onClear));
	}

	void insert(int value) {
		// insert in list
		this->_mylist.push_back(value);

		// fire event
		Core::Worker::Manager::get().add(
			&(this->events.onInsert), // what event is fired
			MyClass::onInsertArguments::Pool::get().create(value) // we create the event arguments
			// the pool will call the "init" method of onInsertArguments with whatever arguments we send to the "create" method of the Pool
		);
	}

	void clear(void) {
		this->_mylist.clear();

		// fire event
		Core::Worker::Manager::get().add(
			&(this->events.onClear), // the event fired
			nullptr // no arguments
		);
	}
};

void MySubscriber {
private:
	MyClass a; // we declare an instance of our class (somewhere)

public:
	MySubscriber():
		a() // events are registered here
	{
		// we subscribe to the event onInsert
		Core::Event::Manager::get().subscribe(
			&(a.events.onInsert), // the event we subscribe to
			std::bind(&MySubscriber::onInsertCallback, this) // the callback (a callable taking a "const Core::Event::IEventArgs*" as arg and returning void)
		);
	}

	// this is called when the event onInsert is fired
	// although remember that this is not done right at the moment that you fire it from MyClass
	// it will be queued to the worker threads and called later
	void onInsertCallback(const Core::Event::IEventArgs *ptr) {
		// reinterpret the arguments
		const MyClass::onInsertArguments* args = reinterpret_cast<const MyClass::onInsertArguments*>(ptr);

		if (args) {
			std::cout << "Just inserted " << args->value << std::endl;
		}
	}
};

```

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

Code from third parties:
- [jsoncpp](https://github.com/open-source-parsers/jsoncpp).
- [utfcpp](http://sourceforge.net/projects/utfcpp/).
- [reader-writer lock with writer priority](http://code.activestate.com/recipes/577803-reader-writer-lock-with-priority-for-writers/) (from python).
- [SHA-512](http://www.zedwood.com/article/cpp-sha512-function).
- [cppformat](http://cppformat.github.io/latest/)
