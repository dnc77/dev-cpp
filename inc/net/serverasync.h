/*
Date: 22 Mar 2019 22:39:13.559867454
File: serverasync.h

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

Purpose: Implements an asynchronous multi threaded basic network server.

Version control
27 Jan 2019 Duncan Camilleri           Initial development
12 Mar 2019 Duncan Camilleri           Introduced user read fd's processing
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __SERVERASYNC_H_DA75534EEBCB171A174507BDEB0AA427__
#define __SERVERASYNC_H_DA75534EEBCB171A174507BDEB0AA427__

// Check for missing includes.
#ifndef __SERVER_H_D20860FF17C3436CF1326F2A1D1C13AA__
#error "serverasync.h: missing include - server"
#elif not defined _GLIBCXX_MUTEX
#error "serverasync.h: missing include - mutex"
#elif not defined _GLIBCXX_THREAD
#error "serverasync.h: missing include - thread"
#endif

//
// Main asynchronous server class
// Implements a tcp based listener which waits for and accepts connections
// on a separate thread as requested.
// Base class server.h implements some of the more basic server functionality.
//
class serverasync : public server
{
public:
   // Constructor/destructor
   serverasync() = delete;
   serverasync(const char* address = nullptr, unsigned short port = 0);
   virtual ~serverasync();

   // Initializations.
   virtual bool init();
   virtual bool term();

   // Connections.
   virtual bool waitForClients();

protected:
   int mMaxTimeoutSec = 30;            // max timeout - 30 sec (incl. term())

   // Clients thread - accepts clients or receives data (run once only).
   once_flag mClientsOnce;             // make sure only one thread is waiting
   thread mClientsThread;              // accept connections and listen for data
   void selectLoop();                  // threaded by waitForClients()
   bool selectProcess(fd_set* pfd);    // processes any sockets requesting attn
};

#endif   // __SERVERASYNC_H_DA75534EEBCB171A174507BDEB0AA427__
