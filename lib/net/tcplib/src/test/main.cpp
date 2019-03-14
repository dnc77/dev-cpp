// Date:    20th January 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
// 28 Jan 2019 Duncan Camilleri           Added basic client test
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
// 03 Mar 2019 Duncan Camilleri           echo client/server *(experimental)*
// 08 Mar 2019 Duncan Camilleri           Comment typo
//

#include <assert.h>
#include <stdio.h>
#include <unistd.h>           // read
#include <memory.h>
#include <netdb.h>
#include <fcntl.h>
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
   printf("%s s <port>: listen on port\n");
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
   // Log info about client.
   printf("client disconnecting: request by %s:%d\n",
      netaddress::address(&pRec->mSockAddr).c_str(),
      netaddress::port(&pRec->mSockAddr)
   );

   // Before acknowledging the connection, check if any data exists and
   // process it.
   fd_set fd;
   FD_ZERO(&fd);
   FD_SET(pRec->mSocket, &fd);

   timeval tv;
   memset(&tv, 0, sizeof(tv));
   int sel = select(pRec->mSocket + 1, &fd, nullptr, nullptr, &tv);
   if (0 == sel || -1 == sel) return;
   
   // Data exists.
   if (!FD_ISSET(pRec->mSocket, &fd))
      return;

   // Since the file descriptor is set, try to receive some data and if that
   // doesn't return anything, then just quit.
   ndstate nds = ndstate::ok;
   pRec->mpXfer->recv(nds);
   while (nds != ndstate::disconnected && nds != ndstate::fail) {
      // Process data.
      size_t size = 0;
      const byte* pBuf = pRec->mpXfer->getRecvBuf(size);
      if (nullptr == pBuf) {
         // No more data received.
         break;
      }

      // Dump the buffer and free it.
      printf("%s:%d: ",
         netaddress::address(&pRec->mSockAddr).c_str(),
         netaddress::port(&pRec->mSockAddr)
      );
      printf("recv>>:");
      dumpbuf(pBuf, size);

      // Clear receive buffer and continue receiving more...
      pRec->mpXfer->clearRecvBuf(size);
      pRec->mpXfer->recv(nds);
   }

   if (nds == ndstate::disconnected || nds == ndstate::fail)
      return;


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
   pRec->mpXfer->recv(nds);

   // Check receive result.
   if (nds == ndstate::disconnected) {
      // Client mae a request to disconnect.
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

   // Data received - get buffer, process and relieve it.
   size_t size = 0;
   const byte* pBuf = pRec->mpXfer->getRecvBuf(size);
   if (nullptr == pBuf) {
      printclient();
      printf("no buffer or space available!\n");
      return;
   }

   // Dump the buffer and free it.
   printclient();
   printf("recv>>:");
   dumpbuf(pBuf, size);

   pRec->mpXfer->clearRecvBuf(size);
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
   // Set standard input as non blocking so that when a read is called, it
   // won't keep waiting for input
   fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

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
   // Set standard input as non blocking so that when a read is called, it
   // won't keep waiting for input
   fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

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

