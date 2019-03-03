// Date:    20th January 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 20 Jan 2019 Duncan Camilleri           Initial development
// 28 Jan 2019 Duncan Camilleri           Added basic client test
// 29 Jan 2019 Duncan Camilleri           netaddress needs utility for pair
// 03 Mar 2019 Duncan Camilleri           echo client/server *(experimental)*
//

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

   // Callback.
   gSvrApp.mpServer->callbackOnConnect(onClientConnect);
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

// Data should be available in the client socket.
// Read it, print it out and free it.
void recvSvrSocket(clientrec* pRec, void* pUserData)
{
   auto printclient = [&]() {
      printf("%s:%d: ",
         netaddress::address(&pRec->mSockAddr).c_str(),
         netaddress::port(&pRec->mSockAddr)
      );
   };

   // Receive available data.
   ndstate nds;
   pRec->mpXfer->recv(nds);

   // Check receive result.
   if (nds == ndstate::disconnected) {
      // Call client disconnect from server end for clean disconnection.
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

// Data inputted. Send it back to client.
void sendSvrKeyboard(clientrec* pRec, void* pUserData)
{
   // Check for a send buffer.
   size_t size = 0;
   byte* pBuf = pRec->mpXfer->getSendBuf(size);
   if (nullptr == pBuf) {
      return;
   }

   // Send buffer available for this client.
   char* pStart = (char*)pBuf;
   char* pEnd = (char*)(pBuf + size);

   // pselect in processClientData sends control to here suggesting
   // there is keyboard input to pick up. At this point, and attempt to
   // fill the send buffer is made. Since stdin is marked as non-blocking
   // for this program, read will return immediately, either empty or with
   // some data.
   size_t res = read(0, pStart, size);
   if (-1 == res) {
      printf("error reading stdin!\n");
   } else if (0 < res) {
      // Got some data. Update send buffer and send it.
      printf("%s:%d: send>>:'%s'\n",
         netaddress::address(&pRec->mSockAddr).c_str(),
         netaddress::port(&pRec->mSockAddr),
         pStart
      );
      pRec->mpXfer->commitSendBuf(res);

      // Send.
      ndstate nds;
      pRec->mpXfer->send(nds);
      switch (nds) {
      case ndstate::disconnected:
         // Call client disconnect from server end for clean disconnection.
         printf("client disconnected\n");
         gSvrApp.mpServer->disconnectClient(pRec);
         break;
      case ndstate::fail:
         printf("send failed\n");
         break;
      default:
         break;
      }
   }
}

// Client processing thread.
void processClientData(clientrec* pRec, void* pUserData)
{
   // Terminate client connection lambda.
   auto terminate = [&]() {
      // Close socket first.
      close(pRec->mSocket);

      // Delete transfer buffer.
      if (nullptr != pRec->mpXfer) {
         delete pRec->mpXfer;
         pRec->mpXfer = 0;
      }
   };

   // Wait for socket data or keyboard data.
   fd_set rfds;
   fd_set fds;
   FD_ZERO(&rfds);
   FD_SET(pRec->mSocket, &rfds);
   FD_SET(0, &rfds);

   int ret = 0;
   do {
      fds = rfds;

      // Wait here.
      ret = select(
         max(pRec->mSocket, 0) + 1,
         &fds, nullptr, nullptr, nullptr
      );

      // Check whether data has arrived from standard input
      // or from the network socket.
      if (ret == -1) {
         terminate();
         printf("unexpected error condition!\n");
         break;
      } else if (ret) {
         if (FD_ISSET(pRec->mSocket, &fds)) {
            // Data available in socket, read and print it.
            recvSvrSocket(pRec, pUserData);
         }

         if (FD_ISSET(0, &fds)) {
            // User input, send it.
            sendSvrKeyboard(pRec, pUserData);
         }
      } else {
         // return 0 (not possible as we don't have a timeout on wait.
         terminate();
         printf("unexpected error condition!\n");
         break;
      }
   } while(pRec->mSocket > 0);
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

   // Call a thread to process client data and exit.
   gSvrApp.mThreads.push_back(thread(processClientData, pRec, pUserData));
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
   // gets called. This will create a data transfer class and initiate
   // a thread to process incoming and outgoing data. Remembering that
   // onClientConnect should not block because that will break the integrity
   // of the server.
   gSvrApp.mpServer->waitForClients();

   // When using a sync server, simulate execution of app below.
   if (!gSvrApp.mSyncServer) {
      printf("waiting...\n");
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

