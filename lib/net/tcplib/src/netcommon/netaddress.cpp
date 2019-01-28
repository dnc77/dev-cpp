// Date:    20th January 2019
// Purpose: Implements address information retrieval.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
//

// Includes
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>         // sockaddr_storage
#include <arpa/inet.h>     // inet_ntop
#include <string>
#include <vector>
#include <netaddress.h>

netaddress::netaddress(sockaddr_storage* paddr, unsigned int len)
{
   memset(&mAddress, 0, sizeof(sockaddr_storage));
   memcpy(&mAddress, paddr, len);
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

   if (AF_INET == mAddress.ss_family) {
      const sockaddr_in& sa = 
         reinterpret_cast<const sockaddr_in&>(mAddress);
      inet_ntop(AF_INET, &sa.sin_addr, pAddress, size - 1);
   } else if (AF_INET6 == mAddress.ss_family) {
      const sockaddr_in6& sa =
         reinterpret_cast<const sockaddr_in6&>(mAddress);
      inet_ntop(AF_INET6, &sa.sin6_addr, pAddress, size);
   }
}

string netaddress::family()
{
   char fam[8];
   family(fam, 8);
   return fam;
}

void netaddress::family(char* pFamily, size_t size)
{
   memset(pFamily, 0, size);
   if (AF_INET == mAddress.ss_family) {
      strncpy(pFamily, "ipv4", size - 1);
   } else if (AF_INET6 == mAddress.ss_family) {
      strncpy(pFamily, "ipv6", size - 1);
   }
}
