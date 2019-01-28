// Date:    20th January 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
//

#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>         // timeval
#include <string>
#include <vector>
#include <thread>
#include <mutex>

#include <helpers.h>
#include <netaddress.h>
#include <netnode.h>
#include <server.h>
#include <serversync.h>
#include <serverasync.h>

void dbltimertest(const int maxsec, const int maxusec)
{
   timeval v;
   v.tv_sec = 0;
   v.tv_usec = 0;
   int n = 0;

   printf("maxsec: %d maxusec: %d sec: %d usec: %d\n",
      maxsec, maxusec, v.tv_sec, v.tv_usec);
   for (; n < 1000; ++n) {
      doubletime(v, maxsec, maxusec);

      printf("maxsec: %d maxusec: %d sec: %d usec: %d\n",
         maxsec, maxusec, v.tv_sec, v.tv_usec);
   }   
}

// Shows all local interfaces using selectserver.
void showlocalinterfaces()
{
   selectserver s(1024);
   s.init();

   // Get all the interfaces available in an address list.
   addresslist list = s.getInterfaces();
   for (int n = 0; n < 2; ++n) {
      netaddress a(&list[n], sizeof(sockaddr_storage));
      printf("%s (%s)\n", a.address().c_str(), a.family().c_str());
   }
}

// Just initializes a server. Synchronous: will block.
// This is a test *only*. Extra work required for a fully operational server.
void serverinit()
{
   server *s = new serversync(nullptr, 1158);
   if (s->init()) {
      printf("server listening %s:%d\n", s->getAddress(), s->getPort());
   } else {
      printf("server init failed!\n");
      return;
   }

   // Will wait forever.
   s->waitForClients();

   // Will not happen.
   s->term();
   delete s;
}

int main(int argc, char** argv)
{
   serverinit();
   return 0;
}

