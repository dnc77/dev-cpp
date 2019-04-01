/*
Date: 22 Mar 2019 22:39:21.672156559
File: netnode.cpp

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

Purpose: Implements a basic network node.

Version control
19 Jan 2019 Duncan Camilleri           Initial development
02 Feb 2019 Duncan Camilleri           Added sockoptAddrReuse
03 Feb 2019 Duncan Camilleri           Added logging support
22 Mar 2019 Duncan Camilleri           Added copyright notice
31 Mar 2019 Duncan Camilleri           Use libraries from same repository

*/

// Includes
#include <stdio.h>         // printf family
#include <memory.h>
#include <sys/types.h>     // socket
#include <sys/socket.h>
#include <netdb.h>         // addrinfo
#include <string>
#include <net/netaddress.h>
#include <net/netnode.h>

extern "C" {
   #include <net/logger.h>                // C does not name mangle
}

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

