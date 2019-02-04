// Date:    20th January 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
// 28 Jan 2019 Duncan Camilleri           Added basic client test
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
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
#include <client.h>
#include <server.h>
#include <serversync.h>
#include <serverasync.h>

extern "C" {
   #include <logger.h>
}

loghdl gLog = 0;

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

// Just initializes a server. Synchronous: will block.
// This is a test *only*. Extra work required for a fully operational server.
void serverinit()
{
   server *s = new serversync(nullptr, 1158);
   s->log(reinterpret_cast<void*>(gLog));

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

#include <memory.h>
void clientissues()
{
   // Set up hints and get info.
   addrinfo* pInfo = nullptr;
   addrinfo hints;
   hints.ai_flags = 0;
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_protocol = 0;
   hints.ai_addrlen = 0;
   hints.ai_addr = nullptr;
   hints.ai_canonname = nullptr;
   hints.ai_next = nullptr;
   if (0 != getaddrinfo("192.168.170.14", "1888", &hints, &pInfo)) {
      printf("getaddrinfo() fail\n");
      return;
   }

   int socksrv = 0;
   addrinfo* pCur = pInfo;
   while (pCur) {
      socksrv =
         socket(AF_INET, SOCK_STREAM, 0
      );

      if (-1 == socksrv) {
         socksrv = 0;
         pCur = pCur->ai_next;
      } else {
         break;
      }
   }

   if (!pCur) {
      printf("socket() fail (socksrv)\n");
      return;
   }

   // Server socket exists. Create compatible client socket now.
   int sockcli = socket(pCur->ai_family, pCur->ai_socktype, 0);
   if (-1 == sockcli) {
      printf("socket() fail (sockcli)\n");
      return;
   }

   if (-1 == ::connect(sockcli, pCur->ai_addr, pCur->ai_addrlen))
      printf("connect() fail\n");
   
   send(socksrv, "Hello server!\n", 14, 0);

   char r[1024];
   memset(r, 0, 1024);
   int n = recv(socksrv, r, 1023, 0);
   if (n == -1) printf("recv() fail\n");
   
   printf("recvd %d bytes\n", n);   
   while (true);
}

// Just initiates a connection with a listening server.
void clientconnect(const char* const server, const unsigned short port)
{
   client c(server, port);
   c.log(reinterpret_cast<void*>(gLog));
   c.setLocal("0.0.0.0", 1084);
   c.init();
   if (c.connect()) { printf("success\n"); }
   else { printf("fail\n"); }

   while (true);
}

int main(int argc, char** argv)
{
   gLog = createLoggerHandle(nullptr, logfull, 1);

   if (!argv[1][0]) {
      destroyLoggerHandle(&gLog);
      printf("args: c/s\n");
      return 1;
   }

   if (argv[1][0] == 'c') {
      clientconnect("192.168.170.11", 1158);
   } else if (argv[1][0] == 's') {
      serverinit();
   } else {
      destroyLoggerHandle(&gLog);
      printf("args: c/s\n");
      return 1;
   }

   destroyLoggerHandle(&gLog);
   return 0;
}

