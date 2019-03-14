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
// 11 Mar 2019 Duncan Camilleri           Added disconnectAllClients()
// 11 Mar 2019 Duncan Camilleri           Added fdset and fdMax functionality
// 11 Mar 2019 Duncan Camilleri           Added onDisconnect callback
// 12 Mar 2019 Duncan Camilleri           Added clientsHead() and clientsTail()
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

   // Set fdset parameters on listening socket.
   FD_ZERO(&mfdAll);
   FD_SET(mSocket, &mfdAll);
   mfdMax = mSocket;

   // Socket is bound to a valid name structure.
   logInfo(mLog, logmore, "server init - complete");
   return true;
}

// Releases the socket.
bool server::term()
{
   mNetAddr.delinfo();

   // Disconnect all clients first.
   disconnectAllClients();

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

   // Remove user fd's.
   mUserReadFds.clear();

   // Clear fdset.
   FD_ZERO(&mfdAll);
   mfdMax = 0;

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

void server::callbackOnDisconnect(servercallback callback)
{
   mOnClientDisconnect = callback;
}

void server::callbackOnData(servercallback callback)
{
   mOnClientData = callback;
}

void server::callbackOnUserReadFd(userfdcallback callback)
{
   mOnUserReadFd = callback;
}

//
// CLIENTS
//

vector<clientrec>::const_iterator server::clientsHead()
{
   return mClients.cbegin();
}

vector<clientrec>::const_iterator server::clientsTail()
{
   return mClients.cend();
}

//
// CONNECTIONS
//

// Disconnects all clients connected to the server.
void server::disconnectAllClients()
{
   // Lock since processing may happen by the user at any time externally.
   mCliLock.lock();
   for (clientrec& cli : mClients) {
      // First call callback if available.
      if (nullptr != mOnClientDisconnect)
         mOnClientDisconnect(&cli, mpUserData);

      // Close the socket.
      close(cli.mSocket);
      cli.mSocket = 0;
   }

   // All clients released.
   mClients.clear();

   // Update fdset and fdmax.
   FD_ZERO(&mfdAll);
   FD_SET(mSocket, &mfdAll);
   mfdMax = mSocket;

   // Release lock.
   mCliLock.unlock();

   // Log message.
   logInfo(mLog, logmore, "server disconnectAllClients - done");
}

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
      // First call callback if available.
      if (nullptr != mOnClientDisconnect)
         mOnClientDisconnect(pClient, mpUserData);

      // Close socket
      int oldsock = pClient->mSocket;
      close(pClient->mSocket);
      pClient->mSocket = 0;

      // Update fdset and fdMax (if this socket was the max).
      FD_CLR(oldsock, &mfdAll);
      if (oldsock == mfdMax)
         updateFdMax();

      // Log message.
      logInfo(mLog, logmore, "server disconnectClient - disconnected %s:%d",
            netaddress::address(&pClient->mSockAddr).c_str(),
            netaddress::port(&pClient->mSockAddr)
      );
   }

   // Remove client.
   mClients.erase(i);
   mCliLock.unlock();
}

//
// FDSET
//

// This allows the user to receive a call back when a select is made on one
// of their file descriptor. To advance on efficiency, given that with a server
// there is the possibility for external additional file descriptors, instead of
// having the user make another expensive select call (with it's loop etc...),
// the server class provides this facility to allow users of the class specify
// additional file descriptors that need to be waited (through select) on.
// Whenever input from any such added descriptors is detected, the server will
// know that the fd does not belong to the listening socket or to any of it's
// clients. The server will call an fdcallback as per user specification to
// enable the processing of that file descriptor. Please note that given the
// nature of the server application, ONLY non blocking callbacks should be used.
// Any blocking callbacks will hinder and possibly disable the server's core
// functional aspects.
void server::captureUserReadFd(int fd, bool enable /*= true*/)
{
   // Invalid file descriptor.
   if (fd < 0) return;

   mCliLock.lock();
   if (enable) {
      if (!FD_ISSET(fd, &mfdAll)) {
         FD_SET(fd, &mfdAll);
         if (fd > mfdMax)
            mfdMax = fd;

         // Add to vector of user fd's.
         addUserReadFd(fd);
      }
   } else {
      FD_CLR(fd, &mfdAll);
      if (fd == mfdMax)
         updateFdMax();

      // Delete from vector of user fd's.
      delUserReadFd(fd);
   }
   mCliLock.unlock();
}

// Update largest socket number.
// Any locking is to be done outside of this call.
void server::updateFdMax()
{
   mfdMax = 0;
   if (mSocket > mfdMax) mfdMax = mSocket;

   // Go through any clients that may exist and check them.
   // Note: The clients lock should be active throughout the
   // time of updateFdMax - hence a lock will not be attempted here.
   for (clientrec& cli : mClients) {
      if (cli.mSocket > mfdMax)
         mfdMax = cli.mSocket;
   }
}

//
// USER READ FDS' MAINTENANCE
//

// This function is needed to ensure that the specified file descriptor
// does not exist elsewhere.
void server::addUserReadFd(int n)
{
   if (n < 0) return;

   // Ensure it's not a socket.
   for (clientrec& cli : mClients) {
      if (n == cli.mSocket)
         return;
   }

   // Ensure it hasn't been added in the past.
   for (int rd : mUserReadFds) {
      if (n == rd)
         return;
   }

   // Can be safely added.
   mUserReadFds.push_back(n);
}

void server::delUserReadFd(int n)
{
   for (auto it = mUserReadFds.begin(); it < mUserReadFds.end(); ++it) {
      if (*it == n) {
         mUserReadFds.erase(it);
         break;
      }
   }
}

