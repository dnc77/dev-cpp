// Date:    17th March 2019
// Purpose: Testing utility for net classes.
//
// Version control
// 17 Mar 2019 Duncan Camilleri           Initial development
//

#include <stdio.h>            // printf
#include <unistd.h>           // close
#include <string.h>           // strerror
#include <errno.h>            // errno
#include <netdb.h>            // protocols
#include <sys/types.h>        // socket
#include <sys/socket.h>       // socket
#include <netinet/ip_icmp.h>  // icmp structs
#include <arpa/inet.h>        // inet_ntop
#include <linux/if_ether.h>   // ETH_P_ALL
#include <sys/time.h>         // helpers
#include <string>             // helpers

#include <helpers.h>          // byte
#include <datastruct/cycbuf.h>
#include "ethframe.h"

using namespace std;

auto gErr = [&](const char* const label, const char* const err) {
   printf("[error %s] %s (%d:%s)\n", label, err, errno, strerror(errno));
};

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

bool ethsniff()
{
   int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
   if (-1 == sock) {
      gErr("init", "could not open socket!");
      return false;
   }

   const int bufsize = 2048;
   byte frame[bufsize];

   // Just receive packet by packet and dump it.
   int rec = 0;
   do {
      memset(&frame, 0, bufsize);

      // Receive bytes.
      rec = recv(sock, frame, bufsize, 0);
      if (-1 == rec) {
         gErr("recv", "recv() failed!");
         close(sock);
         return false;
      }

      // ethhdr analysis
      string protocol;
      ethhdr* pEth = (ethhdr*)frame;
      ethProto(pEth->h_proto, protocol);
      printf("ethframe\n");
      printf("dest: %02x:%02x:%02x:%02x:%02x:%02x\n",
         pEth->h_dest[0], pEth->h_dest[1], pEth->h_dest[2],
         pEth->h_dest[3], pEth->h_dest[4], pEth->h_dest[5]);
      printf("source: %02x:%02x:%02x:%02x:%02x:%02x\n",
         pEth->h_source[0], pEth->h_source[1], pEth->h_source[2],
         pEth->h_source[3], pEth->h_source[4], pEth->h_source[5]);
      printf("protocol: %s\n", protocol.c_str());
      printf("length: %d\n", rec);

      // Dump buffer and empty it.
      hexdump((const char* const)frame, rec, 80);
   } while (rec > 0);

   return true;
}

int main(int argc, char** argv)
{
   return ethsniff() ? 0 : -1;
}
