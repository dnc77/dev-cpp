// Date:    19th January 2019
// Purpose: Implements a basic network node.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
//

// Includes
#include <stdio.h>         // printf family
#include <memory.h>
#include <sys/types.h>     // socket
#include <sys/socket.h>
#include <netdb.h>         // addrinfo
#include <netnode.h>

//
// CONSTRUCTOR/DESCTRUCTOR
//
netnode::netnode(const char* address /*= nullptr*/, unsigned short port /*=0*/)
{
   // Get the address.
   if (nullptr == address) {
      memset(mAddress, 0, mkAddrLen);
   } else {
      strncpy(mAddress, address, mkAddrLen);
   }

   mPort = port;
}

netnode::~netnode()
{
}
