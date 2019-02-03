tcplib

Summary:
This is a simple tcp based network library supporting both IPv4 and IPv6.

This library consists of the following:
a) an asychronous (threaded non-blocking) server class
b) a synchronous (blocking) server class
c) a client

How it works:
A netnode class represents one network node which consists of an IP address and
a socket file descriptor.

A server class inherits from netnode and is used to open the socket for
listening.

A serverasync class inherits yet again from server and implements an
asynchronous client accepting loop (running on a separate thread). Since no
modification to the data within the server is allowed publicly, no mutexes were
required. A socket can be closed during the middle of an accept call without any
repercussions.

A serversync class does the same as serverasync only synchronously. This means
that it will not launch a separate thread to accept client; hence it will block.

A client class inherits from netnode and uses the base class's socket to
represent the server address. The client class also defines a separate client
socket which may or may not be bound to a specific port. If a used port is bound
to the local socket, it cannot establish a connection using that port. In that
case, as per default behaviour, a separate port number is picked up and used by
the system for client purposes.

How to use:
Create an instance of either serversync or serverasync and if init() succeeds,
a call to waitForClients will wait for accepting connections. If using 
serverasync, the program will resume immediately. If using serversync, the call
will block.

Create a client instance with the server's ip address and port as parameters to
the constructor and call init(). At this point, there's the option to choose a
local port by calling setLocal(). Following this, a call to connect() may be
done.

Further extensions to this library are expected.


Thanks

Duncan Camilleri