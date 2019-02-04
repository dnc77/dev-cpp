// Date:    19th January 2019
// Purpose: Implements a basic network node.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
// 31 Jan 2019 Duncan Camilleri           Replaced addrinfo with new netaddress
// 02 Feb 2019 Duncan Camilleri           Added sockoptAddrReuse
// 

#ifndef __NETNODE_H__
#define __NETNODE_H__

// Check for missing includes.
#if not defined _SYS_TYPES_H
#error "netnode.h: missing include - sys/types.h"
#elif not defined _SYS_SOCKET_H
#error "netnode.h: missing include - sys/socket.h"
#elif not defined _NETDB_H
#error "netnode.h: missing include - netdb.h"
#elif not defined __NETADDRESS_H__
#error "netnode.h: missing include - netaddress.h"
#endif

class netnode
{
protected:
   static const short mkAddrLen = 512; // user address length

public:
   netnode() = delete;
   netnode(const char* address = nullptr, unsigned short port = 0);
   virtual ~netnode();

   // Initializations.
   virtual bool init() = 0;
   virtual bool term() = 0;
   void log(void* log)                 { mLog = log; }

   // Accessors.
   const char* const getAddress()      { return mAddress; }
   unsigned short getPort()            { return mPort; }

protected:
   // logging
   void* mLog = nullptr;

   // user connectivity input
   char mAddress[mkAddrLen];
   unsigned short mPort;

   // system connectivity info
   int mSocket = 0;                    // file descriptor
   netaddress mNetAddr;                // address information

protected:
   // Socket options
   bool optAddrReuse(bool enable);
};


#endif      // __NETNODE_H__

