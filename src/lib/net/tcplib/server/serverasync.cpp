/*
Date: 22 Mar 2019 22:39:22.138662982
File: serverasync.cpp

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
29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
03 Feb 2019 Duncan Camilleri           Removed stdout logging (will revise)
03 Feb 2019 Duncan Camilleri           Terminate loop when mSocket <= 0
03 Feb 2019 Duncan Camilleri           Added logging support
24 Feb 2019 Duncan Camilleri           Added OnClientConnect callback support
03 Mar 2019 Duncan Camilleri           Added locking for clients list
11 Mar 2019 Duncan Camilleri           Bug fix moving rc to vector too early
12 Mar 2019 Duncan Camilleri           Introduced user read fd's processing
22 Mar 2019 Duncan Camilleri           Added copyright notice
31 Mar 2019 Duncan Camilleri           Use libraries from same repository

*/

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>                     // timeval
#include <fcntl.h>                        // fcntl
#include <helpers.h>                      // doubletime
#include <netdb.h>
#include <net/netaddress.h>
#include <net/netnode.h>
#include <net/server.h>
#include <net/serverasync.h>

extern "C" {
   #include <net/logger.h>                // C does not name mangle
}

//
// CONSTRUCTOR/DESTRUCTOR
//
serverasync::serverasync(const char* address /*= nullptr*/,
   unsigned short port /*= 0*/)
: server(address, port)
{
}

serverasync::~serverasync()
{
   term();
}

//
// INITIALIZATIONS
//

// Calls on parent.
bool serverasync::init()
{
   if (!server::init())
      return false;

   // Get current socket options.
   int f = fcntl(mSocket, F_GETFL, 0);
   if (-1 == f) {
      server::term();

      logErr(mLog, lognormal, "serverasync init - cannot unblock socket");
      return false;
   }

   // Set the non blocking option.
   if (-1 == fcntl(mSocket, F_SETFL, f | O_NONBLOCK)) {
      server::term();
      
      logErr(mLog, lognormal, "serverasync init - cannot unblock socket");
      return false;
   }

   // Listening socket is non blocking and can proceed.
   logInfo(mLog, logmore, "serverasync init - complete");
   return true;
}

bool serverasync::term()
{
   bool success = true;

   // Close listening socket first. Take note on failure.
   if (!server::term())
      success = false;

   // Wait for the accepting thread to finish. It should finish
   // because the socket was closed. It's highly unlikely that
   // server::term fails and if it does, socket is still reset
   // so accept loop will break out anyway. 
   logInfo(mLog, logmore, "serverasync term - waiting for accept thread");
   mClientsThread.join();

   // Termination done.
   // In the unlikely event that the socket failed to close,
   // mSocket is still set to 0 anyway.
   logInfo(mLog, logmore, "serverasync term - complete");
   return success;
}

//
// CONNECTIONS
//

// Creates a thread which calls the selectLoop function to accept any clients
// that connect to the server or for detecting any data that is coming through
// from any of the already connected clients.
bool serverasync::waitForClients()
{
   try {
      logInfo(mLog, logfull, "serverasync accept - running accept loop once");

      std::call_once(mClientsOnce, [&]() {
         thread t(&serverasync::selectLoop, this);
         mClientsThread = move(t);
      });
   } catch(const std::exception& e) {
      return false;
   }

   // Success.
   return true;
}

// This loop is called by waitForClients as a separate thread via
// mClientsThread. This is only executed once. It's job is to wait
// for client connections or client data to come through. If
// mOnClientData callback is set, will also call the callback to
// notify the server that the client is sending data. Why anyone would
// want to select for client data to come through independently is beyond scope.
// This task is being handled here also in order to centralize all
// wait operations. It is seen as an efficient thing to do.
void serverasync::selectLoop()
{
   // no socket to accept from
   if (mSocket == 0) return;

   // Timer struct.
   timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 0;
   const int maxsec = mMaxTimeoutSec;

   do {
      // Initialize fd sets for select wait operation.
      fd_set fdrd = mfdAll;
      timeval activetime = tv;
      int selected = select(mfdMax + 1, &fdrd, nullptr, nullptr, &activetime);
      if (-1 == selected) {
         // Fail tasks because select failed for some reason.
         logErr(mLog, lognormal, "serverasync select - fail: '%s'",
            strerror(errno));
         break;
      } else if (0 == selected) {
         // No communication received. Double time to wait and wait again.
         logInfo(mLog, logfull,
            "serverasync select - timeout after %ds %dusec",
            tv.tv_sec, tv.tv_usec
         );

         doubletime(tv, maxsec);
         continue;
      }

      // If something came through, process the request based
      // on which socket has received data.
      if (selectProcess(&fdrd)) {
         // Since some data has come through, reset timeout.
         tv.tv_sec = tv.tv_usec = 0;
      }

   // Do not continue selecting if there are no file descriptors to wait on.
   // This happens when all the clients are disconnected and the server has
   // also stopped accepting connections.
   } while (mfdMax > 0);

   // term() signal
   logInfo(mLog, logmore, "serverasync select - term() signal");
}

// If the select loop above detects an input request, it will be processed here.
// This will cater for accepting new clients and also calling the mOnClientData
// callback whenever any client has sent data to the server via that client's
// socket.
// Returns true when a client has been accepted or when a socket is waiting for
// data retrieval.
bool serverasync::selectProcess(fd_set* pfd)
{
   bool actioned = false;

   // First check if a connection request by a new client has been made.
   if (FD_ISSET(mSocket, pfd)) {
      logInfo(mLog, logmore, "serverasync incoming - connection request");

      // A connection request has been made.
      // Prepare a client address information.
      sockaddr_storage ss;
      socklen_t addrsize = sizeof(sockaddr_storage);
      memset(&ss, 0, addrsize);
      sockaddr* psaddr = reinterpret_cast<sockaddr*>(&ss);

      // Accept the connection.
      int sock = accept(mSocket, psaddr, &addrsize);
      if (sock > 0) {
         clientrec rc;
         rc.mSocket = sock;
         rc.mSockAddr = ss;

         // Update maximum and set in mfdAll.
         mCliLock.lock();
         if (sock > mfdMax) mfdMax = sock;
         FD_SET(sock, &mfdAll);

         // If a client has connected, call the OnClientConnect callback.
         if (nullptr != mOnClientConnect)
            mOnClientConnect(&rc, mpUserData);

         mCliLock.unlock();
         logInfo(mLog, lognormal, "serverasync incoming - accepted %s:%d",
            netaddress::address(&ss).c_str(), netaddress::port(&ss)
         );

         // Valid action has been performed.
         mClients.push_back(std::move(rc));
         actioned = true;
      } else {
         logWarn(mLog, lognormal, "serverasync incoming - accept failed");
      }
   }

   // Go through each client socket to see if data has been received
   // and if so, call the onClientData callback.
   if (nullptr != mOnClientData) {
      for (clientrec& cli : mClients) {
         if (FD_ISSET(cli.mSocket, pfd)) {
            logInfo(mLog, logmore,
               "serverasync incoming - data avail. from %s:%d",
               netaddress::address(&cli.mSockAddr).c_str(),
               netaddress::port(&cli.mSockAddr)
            );

            // Callback.
            mOnClientData(&cli, mpUserData);

            // Valid action has been performed.
            actioned = true;
         }
      }
   }

   // Go through user defined fd's and call the user fd's call back.
   if (nullptr != mOnUserReadFd) {
      for (int rd : mUserReadFds) {
         if (FD_ISSET(rd, pfd)) {
            logInfo(mLog, logmore, "serverasync incoming - user input");

            // Callback.
            mOnUserReadFd(this, rd);
         }
      }
   }

   return actioned;
}
