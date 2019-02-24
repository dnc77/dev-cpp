// Date:    19th January 2019
// Purpose: Implements a basic network server.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
// 24 Feb 2019 Duncan Camilleri           Added callback support
//

#ifndef __SERVER_H__
#define __SERVER_H__

// Check for missing includes.
#ifndef _GLIBCXX_VECTOR
#error "server.h: missing include - vector"
#elif not defined _SYS_TYPES_H
#error "server.h: missing include - sys/types.h"
#elif not defined _SYS_SOCKET_H
#error "server.h: missing include - sys/socket.h"
#elif not defined _NETDB_H
#error "server.h: missing include - netdb.h"
#elif not defined __NETADDRESS_H__
#error "server.h: missing include - netaddress.h"
#elif not defined __NETNODE_H__
#error "server.h: missing include - netnode.h"
#endif

using namespace std;

//
// Client record
// A client record represents one client which has connected successfully to
// this server. This is stored as a list in the server.
//

class clientrec;
typedef void (*servercallback)(clientrec* pClient, void* pUserData);

class clientrec
{
public:
   clientrec();

public:
   int mSocket;                        // communicating socket
   sockaddr_storage mSockAddr;         // client address
};

//
// Main server class
// Implements a tcp based listener which accepts connections requested by
// clients.
//

class server : public netnode
{
protected:
   static const short mkBacklog = 5;   // connection back log

public:
   // Constructor/destructor
   server() = delete;
   server(const char* address = nullptr, unsigned short port = 0);
   virtual ~server();

   // Initializations.
   virtual bool init();
   virtual bool term();

   // Callbacks.
   // Callbacks allow the server end to perform various application level
   // functionality whenever client events occur. Callbacks are of type
   // servercallback. When a callback is called, the server will provide
   // two parameters, the pointer to the client record requesting that event
   // as well as some user data (if any) which pertains to the application
   // level and is defined by the user with callbackUserData.
   void callbackUserData(void* pUserData);
   void callbackOnConnect(servercallback callback);

   // Accept connections.
   virtual bool waitForClients() = 0;

protected:
   vector<clientrec> mClients;

   // Callbacks.
   void* mpUserData = nullptr;
   servercallback mOnClientConnect = nullptr;
};


#endif      // __SERVER_H__
