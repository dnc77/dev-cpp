// Date:    28th January 2019
// Purpose: Implements a basic network client.
//
// Version control
// 28 Jan 2019 Duncan Camilleri           Initial development
//

#ifndef __CLIENT_H__
#define __CLIENT_H__

// Check for missing includes.
#if not defined __NETNODE_H__
#error "client.h: missing include - netnode.h"
#endif

using namespace std;

//
// Main client class
// Implements a tcp based client which will establish a connection
// to a desired server. The owning netnode is the address of the 
// server being connected to.
//

class client : public netnode
{
public:
   // Constructor/destructor
   client() = delete;
   client(const char* address, unsigned short port);
   virtual ~client();

   // Initializations
   bool setLocal(const char* const address, unsigned short port);
   virtual bool init();
   virtual bool term();

   // Connection
   bool connect();

private:
   // Local address
   int mLocalSock = 0;
   netaddress mLocal;

   bool bindToLocal();

protected:
   // Socket options
   bool optAddrReuse(bool enable);
};


#endif      // __CLIENT_H__
