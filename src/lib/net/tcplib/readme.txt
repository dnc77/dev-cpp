tcplib

Summary:
This is a simple tcp based network library supporting both IPv4 and IPv6.

This library consists of the following:
a) an asychronous (threaded non-blocking) server class
b) a synchronous (blocking) server class
c) a client
d) a raw data transfer class utilizing a cyclic buffer

How it works:
A netnode class represents one network node which consists of an IP address and
a socket file descriptor.

A server class inherits from netnode and is used to open the socket for
listening.

A serverasync class inherits yet again from server and implements an
asynchronous client accepting loop (running on a separate thread). The client
list on the server is protected by a mutex because external access happens
when a client needs to be disconnected with disconnectClient().

A serversync class does the same as serverasync only synchronously. This means
that it will not launch a separate thread to accept a client; hence it blocks.
Callback functions are used in the server so that when clients connect, they can
be identified at a higher level.

A client class inherits from netnode and uses the base class's socket to
represent the client socket which may or may not be bound to a specific port.
If a used port is bound to the local socket, it cannot establish a connection
using that port. In that case, as per default behaviour, a separate port number
is picked up and used by the system for client purposes.

A data transfer class can be instantiated and assigned to a client in order to
perform data transfer between client and server.
From a server's perspective, since it has a list of clients, each client has its
own socket.

Callbacks are introduced to support the user whenever interesting events happen.
When a client connects, disconnects or is sending data to the server, a callback
can be set to capture the event(s).

The server also provides support to check for extra file descriptors that the
user specifies. This is to prevent the user from having extra waiting calls
making the process much more efficient and streamlined.
If the user has a file descriptor that they are waiting for input from, this
can be assigned to the server and a callback is set. Whenever input is detected
from user descriptors, the callback is called.

When using callbacks, one rule of thumb is to never use blocking functions
otherwise the integrity of the server become compromised. 

How to use:
Create an instance of either serversync or serverasync. If init() succeeds,
any callback functions can be assigned to the server using callbackOnConnect().
User data may also be stored in the server class so that it's subsequently
passed to any callback function. This can be assigned by calling
callbackUserData(). A call to waitForClients will wait for accepting
connections. If using serverasync, the program will resume immediately. If using
serversync, the call will block.

Callback functions can be assigned with the several callback*() functions.
callbackOnUserReadFd can be used to define the callback to call whenever a user
file descriptor is signalled. These descriptors can be added and/or removed by
calling addUserReadFd() and delUserReadFd() respectively.

If a callback function was assigned, as soon as a client connects to the server,
the callback is called. Within the OnClientConnect(), one can assign a data
transfer class (netdataraw) to the client socket to allow communication with
that client. This can be done by netdataraw = netdataraw << socket; Any reads
or writes on the netdataraw buffer will automatically be transmitted over the
socket. To process incoming data, the data transfer class can be used on a call
to onClientData(). When a client disconnects, a call to onClientDisconnect() is
made and at this point, any concluding actions can be done. The socket will be
closed by the server thereafter.

Create a client instance with the server's ip address and port as parameters to
the constructor and call init(). At this point, there's the option to choose a
local port by calling setLocal().
At this point, a data transfer class can be assigned to the client. This can
be done by netdataraw = netdataraw << (netnode&)client;

Following this, a call to connect() may be done and the data transfer class can
be used to transmit data from the client to the server and vice-versa.

The core of this library is close to completion.


Thanks

Duncan Camilleri
