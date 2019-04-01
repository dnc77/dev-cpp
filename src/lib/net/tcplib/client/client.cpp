/*
Date: 22 Mar 2019 22:39:21.325312094
File: client.cpp

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

Purpose: Implements a basic network client.

Version control
28 Jan 2019 Duncan Camilleri           Initial development
29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
03 Feb 2019 Duncan Camilleri           Removed stdout logging (will revise)
03 Feb 2019 Duncan Camilleri           address reuse socket option
04 Feb 2019 Duncan Camilleri           added logging support
23 Feb 2019 Duncan Camilleri           using mSocket correctly no mLocalSock
22 Mar 2019 Duncan Camilleri           Added copyright notice
31 Mar 2019 Duncan Camilleri           Use libraries from same repository

*/

// Includes
#include <string>
#include <vector>
#include <assert.h>                       // debug assertions
#include <unistd.h>                       // close
#include <memory.h>                       // memset
#include <sys/types.h>                    // socket
#include <sys/socket.h>
#include <netdb.h>                        // addrinfo
#include <net/netaddress.h>
#include <net/netnode.h>
#include <net/client.h>

extern "C" {
   #include <net/logger.h>                // C does not name mangle
}

//
// CONSTRUCTOR/DESTRUCTOR
//

client::client(const char* address, unsigned short port)
: netnode(address, port)
{

}

client::~client()
{
   term();
}


//
// INITIALIZATIONS
//

// This function should be called before init. If a defined port or
// address should be used before connecting to a server, then this
// function will attempt to pre-define that.
bool client::setLocal(const char* const address, unsigned short port)
{
   logInfo(mLog, logmore, "client local port - trying with local address %s:%d",
      address, port);

   // Get port string.
   char cport[8];
   memset(cport, 0, 8);
   if (port > 0) sprintf(cport, "%d", port);

   // Get address info.
   if (!mLocal.newinfo(address, port == 0 ? nullptr : cport,
      AI_NUMERICHOST, AF_UNSPEC, SOCK_STREAM, 0)) {
      logErr(mLog, lognormal, "server init - could not find any interfaces");

      return false;
   }

   // Done.
   return (*mLocal != nullptr);
}

// Initializes the socket.
// If a connection already exists, then the initialization will fail.
bool client::init()
{
   // First validate all data.
   // One can't connect to a server if it's not defined.
   if (strlen(mAddress) == 0 || 0 == mPort) {
      logErr(mLog, logmore, "client init - nothing to connect to");
      return false;
   }

   // If socket has been previously initialized, do not re-initialize.
   if (0 != mSocket) return false;

   // Get port string.
   char port[8];
   memset(port, 0, 8);
   sprintf(port, "%d", mPort);

   // Get address info.
   if (!mNetAddr.newinfo(mAddress, port)) {
      logErr(mLog, lognormal, "client init - cannot validate server address");
      return false;
   }

   // Find a valid server address first.
   addrinfo* pCur = mNetAddr;
   while (pCur) {
      // Server socket (netnode::mSocket).
      int svrSocket = socket(pCur->ai_family, pCur->ai_socktype, 0);

      // If socket has been created successfully, update server netaddress
      // with active addrinfo.
      if (-1 == mSocket) {
         mSocket = 0;
         pCur = pCur->ai_next;
      } else {
         close(svrSocket);
         mNetAddr.selectInfo(&pCur);
         break;
      }
   }

   // Failed?
   if (!pCur) {
      mNetAddr.delinfo();
      logCri(mLog, lognormal, "client init - no valid server address!");

      return false;
   }

   // Create compatible client socket now.
   mSocket = socket(pCur->ai_family, pCur->ai_socktype, 0);
   if (-1 == mSocket) {
      mSocket = 0;
      mNetAddr.delinfo();

      logCri(mLog, lognormal, "client init - failed creating client socket!");
      return false;
   }

   // Disable socket address re-use for sockets that have not yet closed.
   // We want to do this because we want to be able to choose another port
   // should a local set port is unavailable.
   optAddrReuse(false);

   // Both client and server sockets created. Attempt to bind to the local
   // specified address through setLocal. Don't worry if this doesn't
   // work. If it doesn't then any local port will be used as opposed to
   // the one specified.
   bindToLocal();
   return true;
}

// Releases the connection.
bool client::term()
{
   mLocal.delinfo();
   mNetAddr.delinfo();

   // Close sockets.
   auto closesock = [](int& sock) -> bool {
      if (sock > 0) {
         if (-1 == close(sock)) {
            sock = 0;
            return false;
         }

         sock = 0;
         return true;
      }
      
      return true;
   };

   // Close socket.
   bool success = true;
   if (!closesock(mSocket)) {
      success = false;
      logWarn(mLog, logmore,
         "client term - failed closing server socket gracefully");
   }

   logInfo(mLog, logmore, "client term - complete");
   return success;
}

//
// CONNECTION
//

// Establish connection with server.
bool client::connect()
{
   if (!mSocket) return false;

   logInfo(mLog, logmore, "client connect: connecting to %s:%d",
      mAddress, mPort);

   // Connect local to server.
   addrinfo* pAddr = mNetAddr;
   if (-1 == ::connect(mSocket, pAddr->ai_addr, pAddr->ai_addrlen)) {
      logErr(mLog, logmore, "client connect: connection to %s:%d failed",
         mAddress, mPort
      );
      
      return false;
   }

   // Connected.
   logInfo(mLog, lognormal, "client connect: connected to %s:%d",
      mAddress, mPort
   );
   return true;
}

//
// LOCAL ADDRESS
//

// After the socket is opened, attempt to bind to a specified
// local address mLocal. This is called only from the init()
// function to ascertain the connecting port which may have been
// specified during a prior call to setLocal.
// No checks will be made for the local socket as these would have
// been done during init().
bool client::bindToLocal()
{
   assert(mSocket > 0);

   // Check if a local address has been assigned through setLocal first.
   addrinfo* paddr = mLocal;

   while (paddr) {
      // Bind local socket.
      if (0 != bind(mSocket, paddr->ai_addr, paddr->ai_addrlen)) {
         paddr = paddr->ai_next;
      } else {
         return true;
      }
   }

   // Bind failed.
   logWarn(mLog, lognormal,
      "client local port - %d already in use", mLocal.port()
   );
   return false;
}

//
// SOCKET OPTIONS
//

// set socket address reuse option for both server and client sockets.
// since client class has more than one socket, need to set the option for all.
bool client::optAddrReuse(bool enable)
{
   // lambda sets/clears option on any sock.
   auto setopt = [&](int sock) -> bool {
      // If no sock available do not fail.
      if (0 >= sock) return true;

      int nEnable = (enable ? 1 : 0);
      return 0 == 
         setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &nEnable, sizeof(int));
   };

   // Go through all sockets and if at least one fails, consider failure.
   bool success = true;
   success = success && setopt(mSocket);

   // Done!
   return success;
}
