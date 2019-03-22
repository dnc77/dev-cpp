/*
Date: 22 Mar 2019 22:39:14.140709360
File: server.h

Copyright Notice
This document is protected by the GNU General Public License v3.0.

This allows for commercial use, modification, distribution, patent and private
use of this software only when the GNU General Public License v3.0 and this
copyright notice are both attached in their original form.

For developer and author protection, the GPL clearly explains that there is no
warranty for this free software and that any source code alterations are to be
shown clearly to identify the original author as well as any subsequent changes
made and by who.

For any questions or ideas, please contact:
github:  https://github(dot)com/dnc77
email:   dnc77(at)hotmail(dot)com
web:     http://www(dot)dnc77(dot)com

Copyright (C) 2000-2019 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Implements a basic network server.

Version control
19 Jan 2019 Duncan Camilleri           Initial development
24 Feb 2019 Duncan Camilleri           Added callback support
25 Feb 2019 Duncan Camilleri           Added data transfer class to clientrec
26 Feb 2019 Duncan Camilleri           Added note about blocking callbacks
03 Mar 2019 Duncan Camilleri           Added disconnectClient
03 Mar 2019 Duncan Camilleri           Added locking for clients list
11 Mar 2019 Duncan Camilleri           Added disconnectAllClients()
11 Mar 2019 Duncan Camilleri           disconnectClient() becomes virtual
11 Mar 2019 Duncan Camilleri           Added fdset and fdMax functionality
11 Mar 2019 Duncan Camilleri           Added onDisconnect callback
11 Mar 2019 Duncan Camilleri           Added support for select on user fd
12 Mar 2019 Duncan Camilleri           Added clientsHead() and clientsTail()
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __SERVER_H_D20860FF17C3436CF1326F2A1D1C13AA__
#define __SERVER_H_D20860FF17C3436CF1326F2A1D1C13AA__

// Check for missing includes.
#ifndef _GLIBCXX_VECTOR
#error "server.h: missing include - vector"
#elif not defined _SYS_TYPES_H
#error "server.h: missing include - sys/types.h"
#elif not defined _SYS_SOCKET_H
#error "server.h: missing include - sys/socket.h"
#elif not defined _NETDB_H
#error "server.h: missing include - netdb.h"
#elif not defined __NETADDRESS_H_6E1F7A0493BF8A3A85BFC6B3372995A7__
#error "server.h: missing include - netaddress.h"
#elif not defined __NETNODE_H_F8E386017993EF09D0EA13C34C3DAD32__
#error "server.h: missing include - netnode.h"
#endif

using namespace std;

// Forward declarations and typedefs.
class clientrec;
class netdataraw;
class server;

typedef void (*servercallback)(clientrec* pClient, void* pUserData);
typedef void (*userfdcallback)(server* pServer, int fd);


//
// Client record
// A client record represents one client which has connected successfully to
// this server. This is stored as a list in the server.
//

class clientrec
{
public:
   clientrec();

public:
   int mSocket;                        // communicating socket
   sockaddr_storage mSockAddr;         // client address
   netdataraw* mpXfer = 0;             // data transfer class
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
   void callbackOnDisconnect(servercallback callback);
   void callbackOnData(servercallback callback);
   void callbackOnUserReadFd(userfdcallback callback);

   // Clients.
   vector<clientrec>::const_iterator clientsHead();
   vector<clientrec>::const_iterator clientsTail();

   // Connections.
   virtual bool waitForClients() = 0;
   virtual void disconnectAllClients();
   virtual void disconnectClient(clientrec* pClient);

   // fdset
   void captureUserReadFd(int fd, bool enable = true);

protected:
   recursive_mutex mCliLock;           // lock to prevent data clients conflicts
   vector<clientrec> mClients;         // list of clients connected
   vector<int> mUserReadFds;           // list of user file descriptors
   fd_set mfdAll;                      // all fdsets to wait on (copied for use)
   int mfdMax = 0;                     // largest socket number (for pselect)

   // fdset
   void updateFdMax();                 // update largest socket number

   // Callbacks.
   // Note: Callbacks should not entertain blocking operations as they
   //       will jeopardize the behaviour of the server.
   //       This applies equally to both servercallback's and fdcallback's.
   void* mpUserData = nullptr;
   servercallback mOnClientConnect = nullptr;
   servercallback mOnClientDisconnect = nullptr;
   servercallback mOnClientData = nullptr;
   userfdcallback mOnUserReadFd = nullptr;

private:
   // User read fds' maintenance.
   void addUserReadFd(int n);
   void delUserReadFd(int n);
};

#endif   // __SERVER_H_D20860FF17C3436CF1326F2A1D1C13AA__

