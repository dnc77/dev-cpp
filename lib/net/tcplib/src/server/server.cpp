// Date:    19th January 2019
// Purpose: Implements a basic network server.
//
// Version control
// 19 Jan 2019 Duncan Camilleri           Initial development
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
#include <netaddress.h>
#include <netnode.h>
#include <server.h>

#include <sys/stat.h>                     // open
#include <fcntl.h>


// 
//           |              |
// ,---.,---.|    ,---.,---.|---     ,---.,---.,---..    ,,---.,---.
// `---.|---'|    |---'|    |        `---.|---'|     \  / |---'|
// `---'`---'`---'`---'`---'`---'    `---'`---'`      `'  `---'`
// Select server is used to determine what addresses are available on
// the local machine for listening. The sole purpose of this class is
// just that.
// NOTE: This class needs to be re-visited as it does not seem to work
// the way it was intended to.

//
// Initialize
// Initialization functions for the server selector.
//

selectserver::selectserver(unsigned short port)
{
   mPort = port;
}

selectserver::~selectserver()
{
   term();
}

// Will try to obtain all available interface in the vector of
// interfaces in this class. If there are already items in this
// vector, for this function to succeed, it has to be emptied.
// This may be done by re-initializing this class (calling term()
// before init()).
bool selectserver::init()
{
   if (!findInterfaces()) return false;

   return true;
}

void selectserver::term()
{
   mActive = nullptr;
   mInterfaces.clear();
}

//
// Interfacing
// All functionality related to the interface selection.
//

// Get all available interfaces that can act as a server.
bool selectserver::findInterfaces()
{
   // Interfaces have already been fetched?
   if (mInterfaces.size() > 0) return false;

   // Get port number.
   if (mPort == 0) return false;
   char port[8];
   sprintf(port, "%d\0", mPort);

   // Locate all interfaces.
   addrinfo hints;
   addrinfo* servers;                           // list of servers available
   memset(&hints, 0, sizeof(addrinfo));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   if (0 != getaddrinfo(0, port, &hints, &servers)) {
      return false;
   }

   // Record all interfaces.
   addrinfo* p = servers;
   while (0 != p) {
      // Create a sockaddr storage and move it to the interfaces structure.
      sockaddr_storage address;
      memset(&address, 0, sizeof(sockaddr_storage));
      memcpy(&address, p->ai_addr, p->ai_addrlen);
      mInterfaces.push_back(address);

      // Next interface.
      p = p->ai_next;
   }

   // All interfaces recorded.
   freeaddrinfo(servers);
   return true;
}

// Gets all interfaces in the vector as constants.
addresslistcref selectserver::getInterfaces() const
{
   // vector<const sockaddr_storage>& 
   return mInterfaces;
}

// Locates the specified user address from the currently loaded address list.
// If found, then the address is selected and returned.
// nullptr returned when no address has been found.
addresscptr selectserver::select(string& address)
{
   // Reset active address.
   mActive = nullptr;

   // Try to find the address in the list of interfaces.
   addresslist::iterator i = mInterfaces.begin();
   for (; i != mInterfaces.end(); ++i) {
      netaddress na(&(*(i)), sizeof(sockaddr_storage));
      if (na.address() == address)
         mActive = &(*i);
   }

   return mActive;
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
   if (0 == mPort) return false;
   sprintf(port, "%d", mPort);

   // If socket has been previously initialized, do not re-initialize.
   if (0 != mSocket) return false;

   // If an address is already registered, clear it.
   // Since socket is free, address should also be free at this point.
   if (nullptr != mpInterface) {
      freeaddrinfo(mpInterface);
      mpInterface = nullptr;
   }

   // Initialize address retrieval hints and get local address.
   if (0 == mAddress[0]) {
      // Should I Look for an address myself?
      addrinfo hints;
      memset(&hints, 0, sizeof(addrinfo));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE;
      if (0 != getaddrinfo(0, port, &hints, &mpInterface)) {
         return false;
      }
   } else {
      // Or will I use the one you've given me?
      addrinfo hints;
      memset(&hints, 0, sizeof(addrinfo));
      hints.ai_family = AF_UNSPEC;
      hints.ai_socktype = SOCK_STREAM;
      if (0 != getaddrinfo(mAddress, port, &hints, &mpInterface)) {
         return false;
      }
   }

   // A valid address is obtained. Open a socket for binding.
   mSocket = socket(mpInterface->ai_family, SOCK_STREAM, 0);
   if (-1 == mSocket) {
      mSocket = 0;
      freeaddrinfo(mpInterface);
      mpInterface = nullptr;

      return false;
   }

   // Update address text.
   sockaddr_storage* psa =
      reinterpret_cast<sockaddr_storage*>(mpInterface->ai_addr);
   netaddress na(psa, mpInterface->ai_addrlen);
   na.address(mAddress, mkAddrLen);
   if (strlen(mAddress) == 0) {
      close(mSocket);
      mSocket = 0;
      freeaddrinfo(mpInterface);
      mpInterface = nullptr;

      return false;
   }

   // Server address info struct is available. Binding time.
   if (0 != bind(mSocket, mpInterface->ai_addr, mpInterface->ai_addrlen)) {
      close(mSocket);
      mSocket = 0;
      freeaddrinfo(mpInterface);
      mpInterface = nullptr;

      return false;
   }

   // Set the socket as listening.
   if (0 != listen(mSocket, mkBacklog)) {
      close(mSocket);
      mSocket = 0;
      freeaddrinfo(mpInterface);
      mpInterface = nullptr;

      return false;
   }

   // Socket is bound to a valid name structure.
   return true;
}

// Releases the socket.
bool server::term()
{
   if (nullptr != mpInterface) freeaddrinfo(mpInterface);
   mpInterface = nullptr;

   // Close listening socket.
   if (mSocket > 0) {
      if (-1 == close(mSocket)) {
         mSocket = 0;
         return false;
      }

      mSocket = 0;
   }

   // Done.
   return true;
}
