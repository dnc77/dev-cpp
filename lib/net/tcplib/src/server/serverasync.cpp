// Date:    27th January 2019
// Purpose: Implements an asynchronous multi threaded basic network server.
//
// Version control
// 27 Jan 2019 Duncan Camilleri           Initial development
//

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
#include <netaddress.h>
#include <netnode.h>
#include <server.h>
#include <serverasync.h>

//
// CONSTRUCTOR/DESCTRUCTOR
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
      return false;
   }

   // Set the non blocking option.
   if (-1 == fcntl(mSocket, F_SETFL, f | O_NONBLOCK)) {
      server::term();
      return false;
   }

   // Listening socket is non blocking and can proceed.
   return true;
}

bool serverasync::term()
{
   bool success = true;

   // Close listening socket first. Take note on failure.
   if (!server::term()) {
      success = false;
   }

   // Wait for the accepting thread to finish. It should finish
   // because the socket was closed. It's highly unlikely that
   // server::term fails and if it does, socket is still reset
   // so accept loop will break out anyway. 
   mAcceptThread.join();

   // Termination done.
   // In the unlikely event that the socket failed to close,
   // mSocket is still set to 0 anyway.
   return success;
}

//
// ACCEPT CONNECTIONS
//

// Creates a thread which calls the acceptLoop function to accept any clients
// that connect to the server.
bool serverasync::waitForClients()
{
   try {
      std::call_once(mAcceptOnce, [&]() {
         thread t(&serverasync::acceptLoop, this);
         mAcceptThread = move(t);
      });
   } catch(const std::exception& e) {
      return false;
   }

   // Success.
   return true;
}

// This loop is called by waitForClients as a separate thread via
// mAcceptThread. This is only executed once.
void serverasync::acceptLoop()
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
         break;
      } else if (0 == selected) {
         // No communication received. Double time to wait and wait again.
         printf("timeout @ %ld %ld\n", (long)tv.tv_sec, (long)tv.tv_usec);
         doubletime(tv, maxsec);
         continue;
      }

      // Otherwise something just arrived...
      // If mSocket has been closed, stop accepting.
      if (0 == mSocket) {
         printf("term\n");
         break;
      }

      // Has a connection request been made?
      if (FD_ISSET(mSocket, &fdrd)) {
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

            printf("client connected\n");
            mClients.push_back(std::move(rc));
            
            // Reset timeout.
            tv.tv_sec = tv.tv_usec = 0;
         }
      }
   // Do not check for mSocket == 0 here as 0 is stdin.
   } while (true);
}


