/*
Date: 22 Mar 2019 22:39:21.209870017
File: main.cpp

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

Purpose: Testing utility for net classes.

Version control
20 Jan 2019 Duncan Camilleri           Initial development
28 Jan 2019 Duncan Camilleri           Added basic client test
29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
03 Mar 2019 Duncan Camilleri           echo client/server *(experimental)*
08 Mar 2019 Duncan Camilleri           Comment typo
22 Mar 2019 Duncan Camilleri           Added copyright notice
23 Mar 2019 Duncan Camilleri           No blocking stdin required
31 Mar 2019 Duncan Camilleri           Use libraries from same repository
01 Apr 2019 Duncan Camilleri           Missing program name in usage for server
02 Apr 2019 Duncan Camilleri           Improvements to disconnect and receive

*/

#include <assert.h>
#include <stdio.h>
#include <unistd.h>           // read
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
#include <net/netaddress.h>
#include <net/netnode.h>
#include <net/client.h>
#include <net/server.h>
#include <net/serversync.h>
#include <net/serverasync.h>
#include <datastruct/cycbuf.h>
#include <net/netdataraw.h>
#include <encode/becode.h>

extern "C" {
   #include <net/logger.h>                // C does not name mangle
}

//
// STRUCTS AND GLOBAL VARS
//

// Server app.
typedef struct _svrappdata {
   bool mSyncServer = false;        // use synch server instead of asynch
   uint16_t mPort = 0;              // port number to listen on
   server* mpServer = nullptr;      // server instance
   vector<thread> mThreads;         // list of threads (one per server)
} serverdata;

// Client app.
typedef struct _clicmd {
   char mServer[256];
   unsigned short mPort;
} clicmd;

typedef struct _appdata {
   clicmd mCmd;
   client* mpClient = nullptr;
   netdataraw mXfer;
} clientdata;

// Logging not used here.
serverdata gSvrApp;
clientdata gCliApp;
loghdl gLog = 0;

// Functions.
int svrmain(int argc, char** argv);
int climain(int argc, char** argv);
void onClientConnect(clientrec* pRec, void* pUserData);
void onClientDisconnect(clientrec* pRec, void* pUserData);
void onClientData(clientrec* pRec, void* pUserData);
void onConsoleInput(server* pServer, int fd);

// 
//      |         |         |    
// ,---.|    ,---.|---.,---.|    
// |   ||    |   ||   |,---||    
// `---|`---'`---'`---'`---^`---'
// `---'                         
// 

//
// UTILS
//

// Output buffer byte by byte to stdout.
void dumpbuf(const byte* pBuf, size_t size)
{
   size_t s = 0;
   while (s < size) putchar((char)pBuf[s++]);
}

//
// MAIN RUN
//

void usage(char* prog)
{
   printf("%s usage:\n", prog);
   printf("%s c <ipaddress> <port>: connect to ipaddress on port\n", prog);
   printf("%s s <port>: listen on port\n", prog);
}

int main(int argc, char** argv)
{
   if (argc < 3) {
      usage(argv[0]);
      return 0;
   }

   char what = argv[1][0];
   if (what == 's') {
      svrmain(argc, argv);
   } else if (what == 'c') {
      climain(argc, argv);
   } else {
      usage(argv[0]);
      return 0;
   }

   return 0;
}

// 
// ,---.,---.,---..    ,,---.,---.
// `---.|---'|     \  / |---'|    
// `---'`---'`      `'  `---'`    
// 

// Get command line options.
bool processSvrCmdline(int argc, char** argv)
{
   if (argc < 3) return false;
   sscanf(argv[2], "%d", &gSvrApp.mPort);

   // Done.
   return true;
}

//
// INITIALIZATIONS
//
bool serverinit()
{
   auto fail = [&]() {
      delete gSvrApp.mpServer;
      gSvrApp.mpServer = nullptr;

      return false;
   };

   // Create server.
   if (gSvrApp.mSyncServer) {
      gSvrApp.mpServer = new serversync("0.0.0.0", gSvrApp.mPort);
   } else {
      gSvrApp.mpServer = new serverasync("0.0.0.0", gSvrApp.mPort);
   }
   if (nullptr == gSvrApp.mpServer) return false;

   // Initialize the server.
   if (!gSvrApp.mpServer->init())
      return fail();

   // Allow console input detection during wait process in server.
   gSvrApp.mpServer->captureUserReadFd(0);

   // Callbacks.
   gSvrApp.mpServer->callbackOnConnect(onClientConnect);
   gSvrApp.mpServer->callbackOnDisconnect(onClientDisconnect);
   gSvrApp.mpServer->callbackOnData(onClientData);
   gSvrApp.mpServer->callbackOnUserReadFd(onConsoleInput);

   return true;
}

void serverterm()
{
   // Destroy server (clients will receive a connection reset).
   delete gSvrApp.mpServer;
   gSvrApp.mpServer = 0;

   // Join on all threads and remove them from the vector.
}

//
// MAIN APP
//

// For the server, this sends a buffer of a certain size to all the clients
// connected to the server.
void svrSendBufToAll(server* pServer, byte* buf, ssize_t in)
{
   for (auto it = pServer->clientsHead(); it != pServer->clientsTail(); ++it) {
      // Get data transfer buffer.
      clientrec& rec = const_cast<clientrec&>(*it);
      netdataraw* pXfer = rec.mpXfer;
      if (!pXfer) {
         printf("send buffer: cannot send to %s:%d\n",
            netaddress::address(&rec.mSockAddr).c_str(),
            netaddress::port(&rec.mSockAddr)
         );
      }

      // Send.
      ssize_t remaining = in;
      while (remaining > 0) {
         ndstate nds;
         size_t bufSize = 0;
         byte* pOut = pXfer->getSendBuf(bufSize);

         // Copy incoming buffer to send buffer.
         int toCopy = min(bufSize, remaining);
         memcpy(pOut, buf, toCopy);

         // Send!
         pXfer->commitSendBuf(toCopy);
         pXfer->send(nds);

         // Check state.
         if (nds == ndstate::disconnected) {
            printf("send buffer: %s:%d has disconnected\n",
               netaddress::address(&rec.mSockAddr).c_str(),
               netaddress::port(&rec.mSockAddr)
            );
            pServer->disconnectClient(&rec);
            break;
         } else if (nds == ndstate::fail) {
            printf("send buffer: failed to send to %s:%d - disconnecting\n",
               netaddress::address(&rec.mSockAddr).c_str(),
               netaddress::port(&rec.mSockAddr)
            );
            pServer->disconnectClient(&rec);
            break;
         }

         // Data sent.
         remaining -= toCopy;
      }
   }
}

// Client connected callback.
void onClientConnect(clientrec* pRec, void* pUserData)
{
   // Log info about client.
   printf("new client connected: %s:%d\n",
      netaddress::address(&pRec->mSockAddr).c_str(),
      netaddress::port(&pRec->mSockAddr)
   );

   // Create a data transfer buffer and wait for data.
   pRec->mpXfer = new netdataraw();
   if (nullptr == pRec->mpXfer) {
      printf("could not create transfer buffer!\n");
      return;
   }

   // Assign data transfer buffer to client socket and
   // wait for data to come through.
   (*pRec->mpXfer) << pRec->mSocket;
}

void onClientDisconnect(clientrec* pRec, void* pUserData)
{
   auto printclient = [&]() {
      printf("%s:%d: ",
         netaddress::address(&pRec->mSockAddr).c_str(),
         netaddress::port(&pRec->mSockAddr)
      );
   };

   // Log info about client.
   printf("client disconnecting: request by %s:%d\n",
      netaddress::address(&pRec->mSockAddr).c_str(),
      netaddress::port(&pRec->mSockAddr)
   );

   // Process any final packet and buffer data.
   size_t size = 0;
   do {
      // Before acknowledging the disconnection request,
      // dump any pending buffer data and check for any remaining buffer data.
      const byte* pBuf = pRec->mpXfer->getRecvBuf(size);
      while (size > 0) {
         if (nullptr == pBuf) break;

         // Dump the buffer and release it from the cyclic buffer.
         printclient();
         printf("recv>>:");
         dumpbuf(pBuf, size);
         pRec->mpXfer->clearRecvBuf(size);

         // Could fetch a buffer again when received data has cycled the buffer.
         pBuf = pRec->mpXfer->getRecvBuf(size);
      }

      // ensure there is no more data for processing from the socket.
      ndstate nds;
      pRec->mpXfer->recv(nds);
      pBuf = pRec->mpXfer->getRecvBuf(size);
   } while (size > 0);

   // Log info about client.
   printf("   disconnected\n");
}

void onClientData(clientrec* pRec, void* pUserData)
{
   auto printclient = [&]() {
      printf("%s:%d: ",
         netaddress::address(&pRec->mSockAddr).c_str(),
         netaddress::port(&pRec->mSockAddr)
      );
   };

   if (!pRec || pRec->mSocket == 0 || pRec->mpXfer == nullptr)
      return;

   // Receive available data.
   ndstate nds;
   do {
      pRec->mpXfer->recv(nds);

      // Check receive result.
      if (nds == ndstate::disconnected) {
         // Client made a request to disconnect.
         // Disconnect also from server for clean disconnection.
         printclient();
         printf("client disconnected\n");
         gSvrApp.mpServer->disconnectClient(pRec);
         return;
      } else if (nds == ndstate::fail) {
         printclient();
         printf("transfer failed!\n");
         return;
      }

      // Data received - get buffer, process and empty it.
      size_t size = 0;
      const byte* pBuf = pRec->mpXfer->getRecvBuf(size);
      while (size > 0) {
         if (nullptr == pBuf) break;

         // Dump the buffer and release it from the cyclic buffer.
         printclient();
         printf("recv>>:");
         dumpbuf(pBuf, size);
         pRec->mpXfer->clearRecvBuf(size);

         // Could fetch a buffer again when received data has cycled the buffer.
         pBuf = pRec->mpXfer->getRecvBuf(size);
      }

   // Keep receiving data until the buffer is not full. This should not happen
   // as the size of an ethernet frame should be smaller than our cyclic buffer.
   } while (nds == ndstate::bufferfull); 
}

// Console input call back from server. This is called when the server's
// select picks up input from one of the user's file descriptors. In this
// case there's only one (the console input).
// When processing input, we must make sure that the process does not
// block at this point otherwise it will break the server.
void onConsoleInput(server* pServer, int fd)
{
   // We only ever use standard in.
   assert(pServer != nullptr && fd == 0);

   // Since this will be sent to all clients, the buffer needs to be copied
   // first.
   byte buf[1024];
   memset(buf, 0, 1024);

   // Since data is available from standard input, read it in
   // and send it to all clients. Ensure no blocking happens at this stage.
   ssize_t in = 0;
   do {
      // Since fd is non-blocking, this will (should) break if there is no data.
      in = read(fd, buf, 1023);

      if (-1 == in) {
         printf("stdin fail\n");
      } else if (in > 0) {
         svrSendBufToAll(pServer, buf, in);
      }
   } while (in == 1023);   // for when there may be more data to read.

   // All data sent to all clients.
}

int svrmain(int argc, char** argv)
{
   // Get command line parameters.
   if (!processSvrCmdline(argc, argv)) {
      usage(argv[0]);
      return 1;
   }

   // Init.
   if (!serverinit()) {
      printf("failed initializing server\n");
      return 1;
   }

   // Wait for clients for ever. When a client connects, onClientConnect
   // gets called. This will create a data transfer class which will allow
   // for data to be transmitted from and to the client.
   // When data is received by the server from the client, onClientData is
   // called.
   // When the client makes a request to disconnect, onClientDisconnect is
   // called.
   gSvrApp.mpServer->waitForClients();

   // When using async server, simulate execution of app below.
   if (!gSvrApp.mSyncServer) {
      while(true);   // (continue running app)
   }

   // TODO: Implement a signal catch which will request program termination.
   // At this point, all threads would be joined for completion and all clients
   // would be disconnected. Then all threads will be removed from the main
   // vector and all allocated objects freed.

   // Term.
   return 0;
}

// 
//      |    o          |    
// ,---.|    .,---.,---.|--- 
// |    |    ||---'|   ||    
// `---'`---'``---'`   '`---'
// 

// Get command line options.
bool processCliCmdline(int argc, char** argv)
{
   memset(&gCliApp.mCmd, 0, sizeof(clicmd));

   if (argc < 4) return false;
   sprintf(gCliApp.mCmd.mServer, "%s", argv[2]);
   sscanf(argv[3], "%d", &gCliApp.mCmd.mPort);

   // Done.
   return true;
}

//
// INITIALIZATIONS
//
bool clientinit()
{
   auto fail = [&]() {
      delete gCliApp.mpClient;
      gCliApp.mpClient = 0;
      
      return false;
   };

   // Create client.
   gCliApp.mpClient = new client(gCliApp.mCmd.mServer, gCliApp.mCmd.mPort);
   if (nullptr == gCliApp.mpClient) return false;

   // Try to connect to server.
   if (!gCliApp.mpClient->init())      return fail();
   if (!gCliApp.mpClient->connect())   return fail();

   return true;
}

void clientterm()
{
   // Destroy client.
   delete gCliApp.mpClient;
   gCliApp.mpClient = 0;
}

//
// CLIENT RECV
//

void cliRecvThread()
{
   ndstate nds;
   
   while (gCliApp.mXfer.recv(nds)) {
      size_t bufsize = 0;
      const byte* buf = gCliApp.mXfer.getRecvBuf(bufsize);

      // Dump the buffer.
      printf("recv=>");
      dumpbuf(buf, bufsize);
      printf("\n");

      // Empty the transfer buffer.
      gCliApp.mXfer.clearRecvBuf(bufsize);
   }

   // Check if there was a disconnection or a failure.
   if (nds == ndstate::disconnected) {
      printf("connection reset\n");
   } else {
      printf("error\n");
   }
}

//
// MAIN
//

int climain(int argc, char** argv)
{
   // Get command line parameters.
   if (!processCliCmdline(argc, argv)) {
      usage(argv[0]);
      return 1;
   }

   // Initialize client.
   if (!clientinit()) {
      printf("Failed to create client!\n");
      return 1;
   }

   // Assign data buffer.
   gCliApp.mXfer << (netnode&)*gCliApp.mpClient;

   // Call listen thread.
   thread clirecv(cliRecvThread);

   // Prepare file descriptor sets to listen to keyboard signals.
   fd_set rfds;
   fd_set fds;
   FD_ZERO(&rfds);
   FD_SET(0, &rfds);

   int ret = 0;
   do {
      fds = rfds;

      // Wait for input first.
      ret = select(1, &fds, nullptr, nullptr, nullptr);
      if (0 > ret) {
         printf("error reading stdin!\n");
         continue;
      } else if (0 == ret) {
         // shouldn't happen on a blocking select.
         printf("no input yet!\n");
         continue;
      }

      // Input found. Prepare send buffer.
      size_t bufsize = 0;
      byte* buf = gCliApp.mXfer.getSendBuf(bufsize);

      // Get input.
      size_t copied = read(0, buf, bufsize);

      // Send.
      ndstate nds;
      gCliApp.mXfer.commitSendBuf(copied);
      gCliApp.mXfer.send(nds);
      
      if (nds == ndstate::disconnected) {
         printf("Server offline.\n");
         break;
      } else if (nds == ndstate::fail) {
         printf("Buffer failed to send.\n");
      }
   } while (true);

   return 0;
}

