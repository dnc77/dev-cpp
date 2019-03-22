/*
Date: 22 Mar 2019 22:39:14.371791930
File: serversync.h

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

Purpose: Implements a synchronous single threaded basic network server.

Version control
27 Jan 2019 Duncan Camilleri           Initial development
12 Mar 2019 Duncan Camilleri           Introduced user read fd's processing
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __SERVERSYNC_H_A3063EF2102BD26E4A8304E809792702__
#define __SERVERSYNC_H_A3063EF2102BD26E4A8304E809792702__

// Check for missing includes.
#ifndef __SERVER_H_D20860FF17C3436CF1326F2A1D1C13AA__
#error "serversync.h: missing include - server"
#elif not defined _GLIBCXX_MUTEX
#error "serversync.h: missing include - mutex"
#endif

//
// Main asynchronous server class
// Implements a tcp based listener which waits for and accepts connections.
// Base class server.h implements some of the more basic server functionality.
//
class serversync : public server
{
public:
   // Constructor/destructor
   serversync() = delete;
   serversync(const char* address = nullptr, unsigned short port = 0);
   virtual ~serversync();

   // Initializations.
   virtual bool init();
   virtual bool term();

   // Accept connections (blocking call).
   virtual bool waitForClients();

protected:
   int mMaxTimeoutSec = 30;            // max timeout - 30 sec (incl. term())

   // Accepting loop.
   bool sessionStop = false;           // indicates the accept loop terminated
   once_flag mAcceptOnce;              // make sure waiting happens only once!
   void selectLoop();                  // called once by waitForClients()
   bool selectProcess(fd_set* pfd);
};

#endif   // __SERVERSYNC_H_A3063EF2102BD26E4A8304E809792702__
