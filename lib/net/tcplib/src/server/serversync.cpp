// Date:    27th January 2019
// Purpose: Implements an asynchronous multi threaded basic network server.
//
// Version control
// 27 Jan 2019 Duncan Camilleri           Initial development
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
// 03 Feb 2019 Duncan Camilleri           Removed stdout logging (will revise)
// 03 Feb 2019 Duncan Camilleri           Terminate loop when mSocket <= 0
// 04 Feb 2019 Duncan Camilleri           added logging support
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

// Calls the acceptLoop function to accept any clients that connect to the
// server. This is a blocking call.
bool serversync::waitForClients()
{
   logInfo(mLog, logfull, "serversync accept - running accept loop once");

   std::call_once(mAcceptOnce, &serversync::acceptLoop, this);
   return true;
}

// This loop is called by waitForClients only once.
void serversync::acceptLoop()
{
   // no socket to accept from
   if (mSocket == 0) return;

   // Initialize fd sets for select wait operation.
   fd_set fdmain;
   fd_set fdrd;
   FD_ZERO(&fdmain);
   FD_SET(mSocket, &fdmain);

   // Timer struct.
   timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 0;
   const int maxsec = mMaxTimeoutSec;

   do {
      // Wait for a connection request.      
      fdrd = fdmain;
      timeval activetime = tv;
      int selected = select(mSocket + 1, &fdrd, nullptr, nullptr, &activetime);
      if (-1 == selected) {
         // Fail tasks because select failed for some reason.
         logErr(mLog, lognormal, "serversync accept - select fail");
         break;
      } else if (0 == selected) {
         // No communication received. Double time to wait and wait again.
         logInfo(mLog, logfull,
            "serversync accept - timeout after %ds %dusec",
            tv.tv_sec, tv.tv_usec
         );

         doubletime(tv, maxsec);
         continue;
      }

      // Otherwise something just arrived...
      // If mSocket has been closed, break out.
      if (0 == mSocket) {
         break;
      }

      // Has a connection request been made?
      if (FD_ISSET(mSocket, &fdrd)) {
         logInfo(mLog, logmore, "serversync accept - connection request");

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
            mClients.push_back(std::move(rc));
            
            // Reset timeout.
            tv.tv_sec = tv.tv_usec = 0;
            
            logInfo(mLog, lognormal, "serversync accept - accepted %s:%d",
               netaddress::address(&ss).c_str(), netaddress::port(&ss)
            );
         } else {
            logWarn(mLog, lognormal, "serversync accept - accept failed");
         }
      }
   } while (mSocket > 0);

   // Update session status - term() signal.
   logInfo(mLog, logmore, "serversync accept - term() signal");
   sessionStop = true;
}

