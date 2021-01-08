// Date:    17th March 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 17 Mar 2019 Duncan Camilleri           Initial development
// 09 Apr 2019 Duncan Camilleri           Re structure to allow for more logging
// 09 Apr 2019 Duncan Camilleri           Added queuing of memory buffers
//

#include <assert.h>           // assert
#include <stdio.h>            // printf
#include <unistd.h>           // close
#include <memory.h>           // ethframe.h
#include <string.h>           // strerror
#include <deque>              // queue
#include <mutex>              // locking for queue
#include <condition_variable>
#include <thread>             // logging thread
#include <errno.h>            // errno
#include <netdb.h>            // protocols
#include <sys/types.h>        // socket
#include <sys/socket.h>       // socket
#include <netinet/ip_icmp.h>  // icmp structs
#include <netinet/ip.h>       // ip header
#include <arpa/inet.h>        // inet_ntop
#include <linux/if_ether.h>   // ETH_P_ALL
#include <sys/time.h>         // helpers
#include <string>             // helpers

#include <helpers.h>          // byte
#include <datastruct/cycbuf.h>
#include "ethlog.h"
#include "ethframe.h"

using namespace std;

int gSock = 0;                // socket receiving eth frames
bool gStop = false;           // used to signal log loop

deque<frameinfo> gEthFrames;  // stores all frames until stdout

thread gLogLoop;              // logging loop thread
mutex gmtxFrm;                // used for synchronous writing to gEthFrames
condition_variable gcvFrm;    // condition variable signaled if frames available


//
// GENERIC LOGGING
//

// Standard error log.
auto gErr = [](const char* const label, const char* const err) {
   printf("[error %s] %s (%d:%s)\n", label, err, errno, strerror(errno));
};

// Hex dump memory output.
void hexdump(const char* const buf, size_t size, unsigned int width)
{
   // Calculate width.
   int charsPerLine = width / 4; // automatic floor :)
   const char* pat = buf;
   const char* const pend = (buf + size);
   while (pat < pend) {
      const char* lineat = pat;
      for (int n = 0; n < charsPerLine; ++n, lineat++) {
         if (lineat < pend) {
            printf("%02x ", *lineat);
         } else {
            printf("   ");
         }
      }

      // Draw ascii characters.
      lineat = pat;
      for (int n = 0; n < charsPerLine; ++n, lineat++) {
         if (lineat < pend) {
            if (!isprint(*lineat)) {
               printf(".");
            } else {
               printf("%c", *lineat);
            }
         } else {
            printf(" ");
         }
      }

      // Next line.
      pat += charsPerLine;
      printf("\n");
   }
}

//
// MAIN LOGGING LOOP
//

void logLoop()
{
   unique_lock<mutex> lock(gmtxFrm, defer_lock);
   while (!gStop) {
      lock.lock();
      while (gEthFrames.empty() && !gStop) {
         gcvFrm.wait(lock);
      }
      lock.unlock();

      if (gStop) break;

      // Frames exist - pick them up and log them.
      frameinfo& fi = gEthFrames.front();
      ethhdr* pEthHdr = (ethhdr*)fi.mBuf;

      // Log the last frame.
      printf("\nethframe\n--------\n\n");
      uint16_t beprotocol = pEthHdr->h_proto;
      logEthFrame(pEthHdr, fi.mSize, beprotocol);

      // Dump IPv4 Header if it is.
      if (beprotocol == ETH_P_IP) {
         printf("\nipv4 header\n-----------\n\n");
         byte* pIpAddr = fi.mBuf + sizeof(ethhdr);
         iphdr* pIP = (iphdr*)pIpAddr;
         logIPV4Header(pIP);
      }

      // Full dump.
      printf("\nhexdump\n-------\n\n");
      hexdump((const char* const)fi.mBuf, fi.mSize, 80);

      // Remove last frame info.
      lock.lock();
      gEthFrames.pop_front();
      lock.unlock();
   }
}

//
// INITIALIZATION
//

bool start()
{
   gSock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
   if (-1 == gSock) {
      gErr("init", "could not open socket!");
      return false;
   }

   return true;
}

void end()
{
   // First close the socket and prevent further packets from coming through.
   if (gSock) {
      close(gSock);
      gSock = 0;
   }

   // Wait for all packets to be logged and removed.
   gStop = true;
   gcvFrm.notify_one();
   gLogLoop.join();

   // EthFrames should be empty by now but empty it just in case.
   gEthFrames.empty();
}


//
// DATA TRANSMISSION
//

bool ethsniff()
{
   if (!start()) return false;

   // Just receive packet by packet as soon as possible.
   int rec = 0;
   do {
      // Create frame info buffer.
      frameinfo fi;

      rec = recv(gSock, fi.mBuf, ETHFRAME_BUFSIZE, 0);
      if (-1 == rec) {
         gErr("recv", "recv() failed!");
         end();
         return false;
      }

      if (0 < rec) {
         fi.mSize = rec;

         // Push to queue.
         gmtxFrm.lock();
         gEthFrames.push_back(std::move(fi));
         gmtxFrm.unlock();

         // Notify logging thread that data is available.
         gcvFrm.notify_one();
      }
   } while (rec > 0);

   // Done.
   end();
   return true;
}

//
// MAIN APP
//

int main(int argc, char** argv)
{
   gLogLoop = thread(logLoop);

   ethsniff();
   return 0;
}
