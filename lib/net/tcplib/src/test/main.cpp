// Date:    20th January 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
// 28 Jan 2019 Duncan Camilleri           Added basic client test
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
//

#include <stdio.h>
#include <memory.h>
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
#include <cycbuf.h>
#include <netdataraw.h>
#include <becode.h>

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

// Callback function for when client connects to server.
void serverOnClientConnected(clientrec* pClient, void* pUserData)
{
   printf("client [%s:%d] connected\n",
      netaddress::address(&pClient->mSockAddr).c_str(),
      netaddress::port(&pClient->mSockAddr)
   );
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

   // Set callback.
   s->callbackOnConnect(serverOnClientConnected);

   // Will wait forever.
   s->waitForClients();

   // Will not happen.
   s->term();
   delete s;
}

// Just initiates a connection with a listening server.
void clientconnect(const char* const server, const unsigned short port)
{
   client c(server, port);
   c.log(reinterpret_cast<void*>(gLog));
   c.setLocal("0.0.0.0", 1084);
   c.init();
   if (!c.connect()) {
      printf("connection failed\n");
      return;
   }

   printf("Connected to port 1084.\n");

   // Send some data.
   netdataraw ndr;
   ndr = ndr << (netnode&)c;
   size_t bufsize = 0;
   byte* pbuf = ndr.getSendBuf(bufsize);
   memset((void*)pbuf, (int)'A', bufsize);
   ndr.commitSendBuf(bufsize);

   ndr.send();

   while(true);
}

// Just runs a client or server for multiple testing purposes.
void clientserver(char what)
{
   gLog = createLoggerHandle(nullptr, logfull, 1);

   if (what == 'c') {
      clientconnect("192.168.170.11", 1158);
   } else if (what == 's') {
      serverinit();
   } else {
      printf("args: c/s\n");
   }

   destroyLoggerHandle(&gLog);
}

int main(int argc, char** argv)
{
   if (argc > 1) clientserver(argv[1][0]);
   return 0;
}
