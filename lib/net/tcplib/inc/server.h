// Date:    19th January 2019
// Purpose: Implements a basic network server.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
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
// Network interface selector
// A class which detects a list of possible servers that can listen locally on
// a port. Since a port may be open at any one time, there might be no
// interface to listen for connection requests on that port. Alternatively,
// there also might be multiple interfaces to choose from. In that case,
// this is where selection takes place.
//
class selectserver
{
public:
   // Initialize
   selectserver() = delete;
   selectserver(unsigned short port);
   virtual ~selectserver();

   bool init();
   void term();

private:
   unsigned short mPort = 0;

   // Interfacing
   // List of interfaces that can listen for connections.
   addresslist mInterfaces;
   addressptr mActive = nullptr;
   bool findInterfaces();

public:
   addresslistcref getInterfaces() const;
   addresscptr select(string& address);
};

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

   // Accept connections.
   virtual bool waitForClients() = 0;

protected:
   vector<clientrec> mClients;
};


#endif      // __SERVER_H__
