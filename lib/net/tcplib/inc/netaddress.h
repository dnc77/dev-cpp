/*
Date: 22 Mar 2019 22:39:14.025607949
File: netaddress.h

Copyright Notice
This document is protected by the GNU General Public License v3.0.

This allows for commercial use, modification, distribution, patent and private
use of this software only when the GNU General Public License v3.0 and this
copyright notice are both attached in their original form.

For developer and author protection, the GPL clearly explains that there is no
warranty for this free software and that any source code alterations are to be
shown clearly to identify the original author as well as any subsequent changes
made and by who.

For any questions or ideas, please contact:
github:  https://github(dot)com/dnc77
email:   dnc77(at)hotmail(dot)com
web:     http://www(dot)dnc77(dot)com

Copyright (C) 2000-2019 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Implements address information retrieval.

Version control
20 Jan 2019 Duncan Camilleri           Initial development
29 Jan 2019 Duncan Camilleri           Added late assignment functionality
31 Jan 2019 Duncan Camilleri           netaddress revamp
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __NETADDRESS_H_6E1F7A0493BF8A3A85BFC6B3372995A7__
#define __NETADDRESS_H_6E1F7A0493BF8A3A85BFC6B3372995A7__

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

#endif   // __NETADDRESS_H_6E1F7A0493BF8A3A85BFC6B3372995A7__
