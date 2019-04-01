/*
Date: 22 Mar 2019 22:39:13.908226358
File: netnode.h

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
31 Jan 2019 Duncan Camilleri           Replaced addrinfo with new netaddress
02 Feb 2019 Duncan Camilleri           Added sockoptAddrReuse
24 Feb 2019 Duncan Camilleri           Added netdataraw support
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __NETNODE_H_F8E386017993EF09D0EA13C34C3DAD32__
#define __NETNODE_H_F8E386017993EF09D0EA13C34C3DAD32__

// Check for missing includes.
#if not defined _SYS_TYPES_H
#error "netnode.h: missing include - sys/types.h"
#elif not defined _SYS_SOCKET_H
#error "netnode.h: missing include - sys/socket.h"
#elif not defined _NETDB_H
#error "netnode.h: missing include - netdb.h"
#elif not defined __NETADDRESS_H_6E1F7A0493BF8A3A85BFC6B3372995A7__
#error "netnode.h: missing include - netaddress.h"
#endif

class netnode
{
friend class netdataraw;

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
   const char* const getAddress()      { return mAddress;   }
   unsigned short getPort()            { return mPort;      }

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


#endif   // __NETNODE_H_F8E386017993EF09D0EA13C34C3DAD32__
