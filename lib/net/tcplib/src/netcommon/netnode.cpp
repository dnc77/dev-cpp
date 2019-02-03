// Date:    19th January 2019
// Purpose: Implements a basic network node.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
// 02 Feb 2019 Duncan Camilleri           Added sockoptAddrReuse
// 

// Includes
#include <stdio.h>         // printf family
#include <memory.h>
#include <sys/types.h>     // socket
#include <sys/socket.h>
#include <netdb.h>         // addrinfo
#include <string>
#include <netaddress.h>
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

//
// SOCKET OPTIONS
//

// enable socket address reuse option
bool netnode::optAddrReuse(bool enable)
{
   if (!mSocket) return false;

   int nEnable = (enable ? 1 : 0);
   return 0 == 
      setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &nEnable, sizeof(int));
}
