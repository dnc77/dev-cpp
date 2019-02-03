// Date:    28th January 2019
// Purpose: Implements a basic network client.
//
// Version control
// 28 Jan 2019 Duncan Camilleri           Initial development
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
// 03 Feb 2019 Duncan Camilleri           Removed stdout logging (will revise)
// 03 Feb 2019 Duncan Camilleri           address reuse socket option
//

// Includes
#include <string>
#include <vector>
#include <assert.h>                       // debug assertions
#include <unistd.h>                       // close
#include <memory.h>                       // memset
#include <sys/types.h>                    // socket
#include <sys/socket.h>
#include <netdb.h>                        // addrinfo
#include <netaddress.h>
#include <netnode.h>
#include <client.h>

//
// CONSTRUCTOR/DESTRUCTOR
//

client::client(const char* address, unsigned short port)
: netnode(address, port)
{

}

client::~client()
{
   term();
}


//
// INITIALIZATIONS
//

// This function should be called before init. If a defined port or
// address should be used before connecting to a server, then this
// function will attempt to pre-define that.
bool client::setLocal(const char* const address, unsigned short port)
{
   // Get port string.
   char cport[8];
   memset(cport, 0, 8);
   if (port > 0) sprintf(cport, "%d", port);

   // Get address info.
   if (!mLocal.newinfo(address, port == 0 ? nullptr : cport,
      AI_NUMERICHOST, AF_UNSPEC, SOCK_STREAM, 0)) {
      return false;
   }

   // Done.
   return (*mLocal != nullptr);
}

// Initializes the socket.
// If a connection already exists, then the initialization will fail.
bool client::init()
{
   // First validate all data.
   // One can't connect to a server if it's not defined.
   if (strlen(mAddress) == 0 || 0 == mPort) return false;

   // If socket has been previously initialized, do not re-initialize.
   if (0 != mSocket) return false;
   assert(0 == mLocalSock);

   // Get port string.
   char port[8];
   memset(port, 0, 8);
   sprintf(port, "%d", mPort);

   // Get address info.
   if (!mNetAddr.newinfo(mAddress, port))
      return false;

   // Find a valid server socket and address first.
   addrinfo* pCur = mNetAddr;
   while (pCur) {
      // Server socket (netnode::mSocket).
      mSocket =
         socket(pCur->ai_family, pCur->ai_socktype, 0
      );

      // If socket has been created successfully, update server netaddress
      // with active addrinfo.
      if (-1 == mSocket) {
         mSocket = 0;
         pCur = pCur->ai_next;
      } else {
         mNetAddr.selectInfo(&pCur);
         break;
      }
   }

   // Failed?
   if (!pCur) {
      mNetAddr.delinfo();
      return false;
   }

   // Server socket exists. Create compatible client socket now.
   mLocalSock = socket(pCur->ai_family, pCur->ai_socktype, 0);
   if (-1 == mLocalSock) {
      close(mSocket);
      mLocalSock = mSocket = 0;
      mNetAddr.delinfo();

      return false;
   }

   // Disable socket address re-use for sockets that have not yet closed.
   // We want to do this because we want to be able to choose another port
   // should a local set port is unavailable.
   optAddrReuse(false);

   // Both client and server sockets created. Attempt to bind to the local
   // specified address through setLocal. Don't worry if this doesn't
   // work. If it doesn't then any local port will be used as opposed to
   // the one specified.
   bindToLocal();
   return true;
}

// Releases the connection.
bool client::term()
{
   mLocal.delinfo();
   mNetAddr.delinfo();

   // Close sockets.
   auto closesock = [](int& sock) -> bool {
      if (sock > 0) {
         if (-1 == close(sock)) {
            sock = 0;
            return false;
         }
         
         sock = 0;
         return true;
      }
      
      return true;
   };

   // Close both sockets.
   return closesock(mSocket) && closesock(mLocalSock);
}

//
// CONNECTION
//

// Establish connection with server.
bool client::connect()
{
   if (!mLocalSock || !mSocket) return false;

   // Connect local to server.
   addrinfo* pAddr = mNetAddr;
   if (-1 == ::connect(mLocalSock, pAddr->ai_addr, pAddr->ai_addrlen))
      return false;

   return true;
}

//
// LOCAL ADDRESS
//

// After the socket is opened, attempt to bind to a specified
// local address mLocal. This is called only from the init()
// function to ascertain the connecting port which may have been
// specified during a prior call to setLocal.
// No checks will be made for the local socket as these would have
// been done during init().
bool client::bindToLocal()
{
   assert(mSocket > 0 && mLocalSock >= 0);

   // Check if a local address has been assigned through setLocal first.
   addrinfo* paddr = mLocal;

   while (paddr) {
      // Bind local socket.
      if (0 != bind(mLocalSock, paddr->ai_addr, paddr->ai_addrlen)) {
         paddr = paddr->ai_next;
      } else {
         return true;
      }
   }

   // Bind failed.
   return false;
}

//
// SOCKET OPTIONS
//

// set socket address reuse option for both server and client sockets.
// since client class has more than one socket, need to set the option for all.
bool client::optAddrReuse(bool enable)
{
   // lambda sets/clears option on any sock.
   auto setopt = [&](int sock) -> bool {
      // If no sock available do not fail.
      if (0 >= sock) return true;

      int nEnable = (enable ? 1 : 0);
      return 0 == 
         setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &nEnable, sizeof(int));
   };

   // Go through all sockets and if at least one fails, consider failure.
   bool success = true;
   success = success && setopt(mSocket);
   success = success && setopt(mLocalSock);

   // Done!
   return success;
}
