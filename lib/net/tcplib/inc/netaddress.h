// Date:    20th January 2019
// Purpose: Implements address information retrieval.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
//

#ifndef __NETADDRESS_H__
#define __NETADDRESS_H__

// Check for missing includes.
#ifndef _GLIBCXX_VECTOR
#error "netaddress.h: missing include - vector"
#elif not defined _GLIBCXX_STRING
#error "netaddress.h: missing include - string"
#elif not defined _NETDB_H
#error "netaddress.h: missing include - netdb.h"
#endif

using namespace std;

// Address and address list types.
using addressref = sockaddr_storage&;
using addresscref = const sockaddr_storage&;
using addressptr = sockaddr_storage*;
using addresscptr = const sockaddr_storage* const;
using addresslist = vector<sockaddr_storage>;
using addresslistref = vector<sockaddr_storage>&;
using addresslistcref = const vector<sockaddr_storage>&;
using addresslistptr = vector<sockaddr_storage>*;
using addresslistcptr = const vector<sockaddr_storage>* const;

class netaddress
{
public:
   netaddress() = delete;
   netaddress(sockaddr_storage* paddr, unsigned int len);
   virtual ~netaddress() { }

   // User information retrieval.
   string address();
   void address(char* pAddress, size_t size);
   string family();
   void family(char* pFamily, size_t size);

private:
   sockaddr_storage mAddress;
};


#endif      // __NETADDRESS_H__

