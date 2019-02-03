// Date:    20th January 2019
// Purpose: Implements address information retrieval.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
// 29 Jan 2019 Duncan Camilleri           Added late assignment functionality
// 31 Jan 2019 Duncan Camilleri           netaddress revamp
//

#ifndef __NETADDRESS_H__
#define __NETADDRESS_H__

// Check for missing includes.
#if not defined _GLIBCXX_STRING
#error "netaddress.h: missing include - string"
#elif not defined _NETDB_H
#error "netaddress.h: missing include - netdb.h"
#endif

using namespace std;

// A class which encapsulates different addresses in the system.
class netaddress
{
public:
   // Construction
   netaddress();
   virtual ~netaddress();

   // addrinfo setup
   bool newinfo(const char* const node, const char* const service,
      const int aiFlags = 0, const int aiFamily = AF_UNSPEC,
      const int aiSockType = SOCK_STREAM, const int protocol = 0);
   void delinfo();

   // sockaddr access
   inline operator addrinfo*()               { return mpActive;               }
   operator sockaddr*();
   operator int();
   addrinfo* operator[](int index);

   // Active addrInfo
   addrinfo* selectInfo(int index);
   int selectInfo(addrinfo** pInfo);
   addrinfo* operator*();

   // User readable address information
   string family();
   void family(char* pFamily, size_t size);
   string address();
   void address(char* pAddress, size_t size);
   unsigned short port();

private:
   addrinfo* mpInfo = nullptr;
   addrinfo* mpActive = nullptr;
   unsigned int mActiveIdx;

public:
   // Static address translation
   static string family(const sockaddr_storage* pss);
   static string address(const sockaddr_storage* pss);
   static unsigned short port(const sockaddr_storage* pss);
};

#endif      // __NETADDRESS_H__
