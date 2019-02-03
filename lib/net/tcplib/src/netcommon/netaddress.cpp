// Date:    20th January 2019
// Purpose: Implements address information retrieval.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
// 29 Jan 2019 Duncan Camilleri           Added late assignment functionality
// 31 Jan 2019 Duncan Camilleri           netaddress revamp
//

// Includes
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>         // sockaddr_storage
#include <arpa/inet.h>     // inet_ntop
#include <string>
#include <vector>
#include <utility>
#include <netaddress.h>

netaddress::netaddress()
: mpActive(mpInfo), mActiveIdx(0)            // activate first item by default
{
}

netaddress::~netaddress()
{
   delinfo();
}


//
// ADDRINFO SETUP
// When newInfo is called, a new mpInfo is set up by calling getaddrinfo.
// If unsuccessful, mpNewInfo will be reset to nullptr. The local addrinfo
// has to be freed with freeaddrinfo. The destructor will take care of that but
// a call to delinfo() will also do. If a previous mNewInfo was allocated, it
// will be freed prior to re-allocating a new one automatically.
//

bool netaddress::newinfo(const char* const node,
   const char* const service,
   const int aiFlags /*= 0*/,
   const int aiFamily /*= AF_UNSPEC*/,
   const int aiSockType /*= SOCK_STREAM*/,
   const int protocol /*= 0*/)
{
   // Delete automatically any previous information.
   delinfo();

   // Set up hints and get info.
   addrinfo hints;
   hints.ai_flags = aiFlags;
   hints.ai_family = aiFamily;
   hints.ai_socktype = aiSockType;
   hints.ai_protocol = protocol;
   hints.ai_addrlen = 0;
   hints.ai_addr = nullptr;
   hints.ai_canonname = nullptr;
   hints.ai_next = nullptr;
   if (0 != getaddrinfo(node, service, &hints, &mpInfo)) {
      return false;
   }

   // Done!
   mpActive = mpInfo;
   return true;
}

void netaddress::delinfo()
{
   if (mpInfo) {
      freeaddrinfo(mpInfo);
      mpActive = mpInfo = nullptr;
      mActiveIdx = 0;
   }
}

//
// SOCKADDR ACCESS
// Various means of accessing sockaddr members in mpInfo.
//

// Returns active sockaddr*. Note that mpActive is always set to
// mpInfo when no previous selectInfo has been called.
netaddress::operator sockaddr*()
{
   return mpActive ? mpActive->ai_addr : nullptr;
}

// Returns active sockaddr* length. Note that mpActive is always set to
// mpInfo when no previous selectInfo has been called.
netaddress::operator int()
{
   return mpActive ? mpActive->ai_addrlen : 0;
}

// Get addrinfo at index offset or null in case of invalid index
// or non existing data. index 0 is the first (root) addrinfo.
addrinfo* netaddress::operator[](int index)
{
   if (!mpInfo) return nullptr;

   addrinfo* pRet = mpInfo;
   while (--index >= 0) {
      if (pRet->ai_next == nullptr) return nullptr;
      pRet = pRet->ai_next;
   }

   // Gone through and reached target - return.
   return pRet;
}

//
// ACTIVE ADDRINFO
// The active addrinfo is a pointer to a user chosen addrinfo
// after some operations are performed. This is stored for caching
// purposes to avoid looking for it by traversing the linked list everytime.

// Selects the info at index and returns it. If index is invalid, the active
// address is not changed and nullptr is returned.
addrinfo* netaddress::selectInfo(int index)
{
   addrinfo* active = (*this)[index];
   if (active) {
      mpActive = active;
      mActiveIdx = index;
   } else {
      return nullptr;
   }

   return mpActive;
}

// Selects the matching addrinfo and returns it's index.
// If the addrinfo provided does not exist, the active address is not changed
// and -1 is returned.
int netaddress::selectInfo(addrinfo** ppInfo)
{
   // Locate item.
   int index = 0;
   addrinfo* p = mpInfo;
   while (p != nullptr && *ppInfo != p) {
      p = p->ai_next;
      index++;
   }

   // Update index and active item.
   if (p == nullptr) {
      return -1;
   }
   
   // Found item - update active item.
   mpActive = p;
   mActiveIdx = index;
   return index;
}

// Will fetch the active addrinfo in the class.
addrinfo* netaddress::operator*()
{
   return mpActive;
}

//
// USER READABLE ADDRESS INFORMATION
//

string netaddress::family()
{
   char fam[8];
   family(fam, 8);
   return fam;
}

void netaddress::family(char* pFamily, size_t size)
{
   memset(pFamily, 0, size);
   if (nullptr == mpActive) return;

   if (AF_INET == mpActive->ai_addr->sa_family) {
      strncpy(pFamily, "ipv4", size - 1);
   } else if (AF_INET6 == mpActive->ai_addr->sa_family) {
      strncpy(pFamily, "ipv6", size - 1);
   }
}

// Retrieves a string with a user readable name of the address.
string netaddress::address()
{
   // Allocate a buffer large enough to cater for any size.
   char addr[INET6_ADDRSTRLEN];
   address(addr, INET6_ADDRSTRLEN);
   return addr;
}

// Fills pAddress with the address of the local sockaddr with up to
// maxLen chars.
void netaddress::address(char* pAddress, size_t size)
{
   memset(pAddress, 0, size);
   if (nullptr == mpActive) return;

   if (AF_INET == mpActive->ai_addr->sa_family) {
      const sockaddr_in* psa = 
         reinterpret_cast<const sockaddr_in*>(mpActive->ai_addr);
      inet_ntop(AF_INET, &psa->sin_addr, pAddress, size - 1);
   } else if (AF_INET6 == mpActive->ai_addr->sa_family) {
      const sockaddr_in6* psa =
         reinterpret_cast<const sockaddr_in6*>(mpActive->ai_addr);
      inet_ntop(AF_INET6, &psa->sin6_addr, pAddress, size);
   }
}

// Gets the port of the specified address.
unsigned short netaddress::port()
{
   if (nullptr == mpActive) return 0;

   if (AF_INET == mpActive->ai_addr->sa_family) {
      const sockaddr_in* psa = 
         reinterpret_cast<const sockaddr_in*>(mpActive->ai_addr);
      return ntohs(psa->sin_port);
   } else if (AF_INET6 == mpActive->ai_addr->sa_family) {
      const sockaddr_in6* psa =
         reinterpret_cast<const sockaddr_in6*>(mpActive->ai_addr);
      return ntohs(psa->sin6_port);
   }

   return 0;
}

//
// STATIC ADDRESS TRANSLATION
//

string netaddress::family(const sockaddr_storage* pss)
{
   if (AF_INET == pss->ss_family) {
      return "ipv4";
   } else if (AF_INET6 == pss->ss_family) {
      return "ipv6";
   }

   return "";
}

string netaddress::address(const sockaddr_storage* pss)
{
   char address[1024];
   memset(address, 0, 1024);
   if (nullptr == pss) return "";

   if (AF_INET == pss->ss_family) {
      const sockaddr_in* pin = reinterpret_cast<const sockaddr_in*>(pss);
      inet_ntop(AF_INET, &pin->sin_addr, address, 1023);
   } else if (AF_INET6 == pss->ss_family) {
      const sockaddr_in6* pin = reinterpret_cast<const sockaddr_in6*>(pss);
      inet_ntop(AF_INET6, &pin->sin6_addr, address, 1023);
   }

   return address;
}

unsigned short netaddress::port(const sockaddr_storage* pss)
{
   if (nullptr == pss) return 0;

   if (AF_INET == pss->ss_family) {
      const sockaddr_in* pin = reinterpret_cast<const sockaddr_in*>(pss);
      return ntohs(pin->sin_port);
   } else if (AF_INET6 == pss->ss_family) {
      const sockaddr_in6* pin = reinterpret_cast<const sockaddr_in6*>(pss);
      return ntohs(pin->sin6_port);
   }

   return 0;
}
