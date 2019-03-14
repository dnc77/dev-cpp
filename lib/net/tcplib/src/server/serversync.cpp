// Date:    27th January 2019
// Purpose: Implements a synchronous basic network server.
//
// Version control
// 27 Jan 2019 Duncan Camilleri           Initial development
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
// 03 Feb 2019 Duncan Camilleri           Removed stdout logging (will revise)
// 03 Feb 2019 Duncan Camilleri           Terminate loop when mSocket <= 0
// 04 Feb 2019 Duncan Camilleri           Added logging support
// 24 Feb 2019 Duncan Camilleri           Added OnClientConnect callback support
// 03 Mar 2019 Duncan Camilleri           Added locking for clients list
// 08 Mar 2019 Duncan Camilleri           Purpose of file updated
// 11 Mar 2019 Duncan Camilleri           Bug fix moving rc to vector too early
// 14 Mar 2019 Duncan Camilleri           Introduced user read fd's processing
// 
#include <string>
#include <vector>
#include <mutex>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>                     // timeval
#include <fcntl.h>                        // fcntl
#include <helpers.h>                      // doubletime
#include <netdb.h>
#include <netaddress.h>
#include <netnode.h>
#include <server.h>
#include <serversync.h>

extern "C" {
   #include <logger.h>                    // C does not name mangle
}

//
// CONSTRUCTOR/DESCTRUCTOR
//
serversync::serversync(const char* address /*= nullptr*/,
   unsigned short port /*= 0*/)
: server(address, port)
{
}

serversync::~serversync()
{
   term();
}

//
// INITIALIZATIONS
//

// Calls on parent.
bool serversync::init()
{
   if (!server::init())
      return false;

   // Get current socket options.
   int f = fcntl(mSocket, F_GETFL, 0);
   if (-1 == f) {
      server::term();

      logErr(mLog, lognormal, "serversync init - cannot unblock socket");
      return false;
   }

   // Set the non blocking option.
   if (-1 == fcntl(mSocket, F_SETFL, f | O_NONBLOCK)) {
      server::term();

      logErr(mLog, lognormal, "serversync init - cannot unblock socket");
      return false;
   }

   // Listening socket is non blocking and can proceed.
   logInfo(mLog, logmore, "serversync init - complete");
   return true;
}

bool serversync::term()
{
   bool success = true;

   // Close listening socket first. Take note on failure.
   if (!server::term()) {
      success = false;
   }

   // When the socket is closed after term, the accepting loop is still
   // running. It's ideal to wait for the loop to exit. To do that,
   // sessionStop has been introduced. When set to true, then the accepting
   // loop has completed.
   logInfo(mLog, logmore, "serversync term - waiting for accept loop");
   while (!sessionStop);

   // Termination done.
   // In the unlikely event that the socket failed to close,
   // mSocket is still set to 0 anyway.
   logInfo(mLog, logmore, "serversync term - complete");
   return success;
}

//
// ACCEPT CONNECTIONS
//

// Calls the selectLoop function to accept any clients that connect to the
// server. This is a blocking call.
bool serversync::waitForClients()
{
   logInfo(mLog, logfull, "serversync accept - running accept loop once");

   std::call_once(mAcceptOnce, &serversync::selectLoop, this);
   return true;
}

// This loop is called by waitForClients only once.
void serversync::selectLoop()
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
         logErr(mLog, lognormal, "serversync accept - select fail");
         break;
      } else if (0 == selected) {
         // No communication received. Double time to wait and wait again.
         logInfo(mLog, logfull,
            "serversync select - timeout after %ds %dusec",
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

   // Update session status - term() signal.
   logInfo(mLog, logmore, "serversync select - term() signal");
   sessionStop = true;
}

// If the select loop above detects an input request, it will be processed here.
// This will cater for accepting new clients and also calling the mOnClientData
// callback whenever any client has sent data to the server via that client's
// socket.
// Returns true when a client has been accepted or when a socket is waiting for
// data retrieval.
bool serversync::selectProcess(fd_set* pfd)
{
   bool actioned = false;

   // First check if a connection request by a new client has been made.
   if (FD_ISSET(mSocket, pfd)) {
      logInfo(mLog, logmore, "serversync incoming - connection request");

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
         if (sock > mfdMax) mfdMax = sock;
         FD_SET(sock, &mfdAll);

         // If a client has connected, call the OnClientConnect callback.
         if (nullptr != mOnClientConnect)
            mOnClientConnect(&rc, mpUserData);

         logInfo(mLog, lognormal, "serversync incoming - accepted %s:%d",
            netaddress::address(&ss).c_str(), netaddress::port(&ss)
         );

         // Valid action has been performed.
         mClients.push_back(std::move(rc));
         actioned = true;
      } else {
         logWarn(mLog, lognormal, "serversync incoming - accept failed");
      }
   }

   // Go through each client socket to see if data has been received
   // and if so, call the onClientData callback.
   if (nullptr != mOnClientData) {
      for (clientrec& cli : mClients) {
         if (FD_ISSET(cli.mSocket, pfd)) {
            logInfo(mLog, logmore,
               "serversync incoming - data avail. from %s:%d",
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
            logInfo(mLog, logmore, "serversync incoming - user input");

            // Callback.
            mOnUserReadFd(this, rd);
         }
      }
   }

   return actioned;
}
