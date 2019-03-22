/*
Date: 22 Mar 2019 22:39:14.604599620
File: client.h

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
23 Feb 2019 Duncan Camilleri           using mSocket correctly no mLocalSock
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __CLIENT_H_BEEE66FB5E4AEAA9EC8626AF70B1ECAB__
#define __CLIENT_H_BEEE66FB5E4AEAA9EC8626AF70B1ECAB__

// Check for missing includes.
#if not defined __NETNODE_H_F8E386017993EF09D0EA13C34C3DAD32__
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
   netaddress mLocal;

   bool bindToLocal();

protected:
   // Socket options
   bool optAddrReuse(bool enable);
};

#endif   // __CLIENT_H_BEEE66FB5E4AEAA9EC8626AF70B1ECAB__
