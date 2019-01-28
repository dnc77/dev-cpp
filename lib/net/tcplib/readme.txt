netlib

Summary:
This is a simple tcp based network library.
Currently there's two server implementations supported in this library.

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

How to use:
Create an instance of either serversync or serverasync and if init() succeeds,
a call to waitForClients will wait for accepting connections. If using 
serverasync, the program will resume immediately. If using serversync, the call
will block.

Further extensions to this library are expected.


Thanks

Duncan Camilleri
