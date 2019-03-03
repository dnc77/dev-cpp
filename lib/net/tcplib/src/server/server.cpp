// Date:    19th January 2019
// Purpose: Implements a basic network server.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
// 31 Jan 2019 Duncan Camilleri           New netaddress changes
// 03 Feb 2019 Duncan Camilleri           Address reuse socket option
// 03 Feb 2019 Duncan Camilleri           Added logging support
// 24 Feb 2019 Duncan Camilleri           Added callback support
// 03 Mar 2019 Duncan Camilleri           Added disconnectClient
// 03 Mar 2019 Duncan Camilleri           Added locking for clients list
//

// Includes
#include <stdio.h>
#include <unistd.h>                       // close
#include <memory.h>
#include <sys/types.h>                    // socket
#include <sys/socket.h>
#include <arpa/inet.h>                    // inet_ntop
#include <netdb.h>                        // addrinfo
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>                      // std::find
#include <netaddress.h>
#include <netnode.h>
#include <server.h>

#include <sys/stat.h>                     // open
#include <fcntl.h>

extern "C" {
   #include <logger.h>                    // C does not name mangle
}

// 
//      |    o          |                                     |
// ,---.|    .,---.,---.|---     ,---.,---.,---.,---.,---.,---|
// |    |    ||---'|   ||        |    |---'|    |   ||    |   |
// `---'`---'``---'`   '`---'    `    `---'`---'`---'`    `---'
// 
clientrec::clientrec()
: mSocket(0)
{
   memset(&mSockAddr, 0, sizeof(sockaddr_storage));
}

//
// ,---.,---.,---..    ,,---.,---.
// `---.|---'|     \  / |---'|
// `---'`---'`      `'  `---'`
// 

//
// CONSTRUCTOR/DESTRUCTOR
//
server::server(const char* address /*= nullptr*/, unsigned short port /*= 0*/)
: netnode(address, port)
{
}

server::~server()
{
   term();
}

//
// INITIALIZATIONS
//

// Initializes the socket.
bool server::init()
{
   // First validate all data.
   // One can't have a server when they don't know on which port to listen to.
   char port[8];
   if (0 == mPort) {
      logErr(mLog, lognormal, "server init - invalid port (%d)", mPort);
      return false;
   }
   sprintf(port, "%d", mPort);

   // If socket has been previously initialized, do not re-initialize.
   if (0 != mSocket) return false;

   // If an address is already registered, clear it.
   // Since socket is free, address should also be free at this point.
   if (nullptr != *mNetAddr) mNetAddr.delinfo();

   // Initialize address retrieval hints and get local address.
   // Either use the one provided or find one by not providing an address.
   if (0 == mAddress[0]) {
      if (!mNetAddr.newinfo(nullptr, port,
         AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0)) {
         logErr(mLog, lognormal, "server init - could not find any interfaces");
         return false;
      }
   } else {
      if (!mNetAddr.newinfo(mAddress, port,
         0, AF_UNSPEC, SOCK_STREAM)) {
         logErr(mLog, lognormal,
            "server init - failed locating interfaces for %s(%d)",
            mAddress, mPort
         );
         
         return false;
      }
   }

   // A valid address is obtained. Open a socket for binding.
   addrinfo* paddr = *mNetAddr;
   mSocket = socket(paddr->ai_family, paddr->ai_socktype, paddr->ai_protocol);
   if (-1 == mSocket) {
      mSocket = 0;
      mNetAddr.delinfo();
      logCri(mLog, lognormal, "server init - failed creating server socket!");

      return false;
   }

   // Enable socket address re-use for sockets that have not yet closed.
   optAddrReuse(true);

   // Update address text.
   mNetAddr.address(mAddress, mkAddrLen);
   if (strlen(mAddress) == 0) {
      mNetAddr.delinfo();
      close(mSocket);
      mSocket = 0;

      logErr(mLog, lognormal, "server init - no server address detected");
      return false;
   }

   // ...and port
   mPort = mNetAddr.port();

   // Server address info struct is available. Binding time.
   if (0 != bind(mSocket, paddr->ai_addr, paddr->ai_addrlen)) {
      mNetAddr.delinfo();
      close(mSocket);
      mSocket = 0;

      logErr(mLog, lognormal, "server init - could not bind server socket");
      return false;
   }

   // Set the socket as listening.
   if (0 != listen(mSocket, mkBacklog)) {
      mNetAddr.delinfo();
      close(mSocket);
      mSocket = 0;

      logErr(mLog, lognormal, "server init - could not listen");
      return false;
   }

   // Socket is bound to a valid name structure.
   logInfo(mLog, logmore, "server init - complete");
   return true;
}

// Releases the socket.
bool server::term()
{
   mNetAddr.delinfo();

   // Close listening socket.
   if (mSocket > 0) {
      if (-1 == close(mSocket)) {
         mSocket = 0;
         logWarn(mLog, logmore,
            "server term - failed closing socket gracefully"
         );

         return false;
      }

      mSocket = 0;
   }

   // Done.
   logInfo(mLog, logmore, "server term - complete");
   return true;
}

//
// CALLBACKS
//

void server::callbackUserData(void* pUserData)
{
   mpUserData = pUserData;
}

void server::callbackOnConnect(servercallback callback)
{
   mOnClientConnect = callback;
}

//
// CONNECTIONS
//

// Disconnects and removes a client from the list.
void server::disconnectClient(clientrec* pClient)
{
   // Comparison function.
   auto compare =
      [&](const clientrec& c) -> bool {
         return c.mSocket == pClient->mSocket;
      };

   // Find disconnected client first.
   mCliLock.lock();
   vector<clientrec>::iterator i = 
      std::find_if(mClients.begin(), mClients.end(), compare);

   // Ensure socket is closed.
   if (pClient->mSocket > 0) {
      close(pClient->mSocket);
      pClient->mSocket = 0;
   }
   
   // Remove from vector.
   mClients.erase(i);
   mCliLock.unlock();
}
