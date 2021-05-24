/*
Date: 14 May 2021 13:41:53.060139539
File: pcapsniff.cpp.cpp

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

Copyright (C) 2021 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Version control
14 May 2021 Duncan Camilleri           Initial development
*/

extern "C" {
   #include "logger.h"
}

#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <linux/tcp.h>
#include <pcap.h>
#include <dlt.h>
#include "http.h"
#include "structs.h"
#include "pcapsniff.h"

//
// Globals
//
Sniff gSniff;

//
// CMD LINE PROCESSING 
//

// Shows all available options.
void usage()
{
   printf("Usage:\n");
   printf("pcapsniff <-kkeyfile> <-iinterface> [-l]\n");
   printf("-i<interface>      use selected interface\n");
   printf("-l                 list all interfaces\n");
   printf("-k<keyfile>        read keywords from file\n");
   printf("-v                 verbose - show more stuff\n");
   printf("\n");
}

// Show selected options.
void showCmdline(Sniff* pSniff)
{
   printf("Using interface: %s [%s]\n",
      pSniff->msd.mLocalIP, pSniff->mCmdline.mInterface
   );
   printf("Using key file: [%s]\n", pSniff->mCmdline.mKeyFile);

   if (pSniff->mCmdline.mVerbose) printf("Verbose mode\n");
}

// Will process command line arguments and if the process can proceed with the
// given info, will return true. If for some reason, processing can't resume,
// this will return false and it's expected that usage info be printed out to
// the user to establish command line guidelines.
bool getCmdLineOpts(int argc, char** argv, CmdLine& cmdline)
{     
   // Initialize command line info.
   memset(&cmdline, 0, sizeof(CmdLine));

   // Validate parameters.
   if (argc < 2) return false;

   while (argc > 0) {
      // Next!
      argc--;
      // Check command line parameters.
   
      // Select interface name.
      if (strncasecmp(argv[argc], "-i", 2) == 0) {
         strncpy(cmdline.mInterface, &argv[argc][2], 63);
      }      

      // List interfaces.
      if (strncasecmp(argv[argc], "-l", 2) == 0) {
         cmdline.mShowInterfaces = true;
      }

      // Key file.
      if (strncasecmp(argv[argc], "-k", 2) == 0) {
         strncpy(cmdline.mKeyFile, &argv[argc][2], 255);
         FILE* fTest = fopen(cmdline.mKeyFile, "rt");
         if (fTest) {
            fclose(fTest);
         } else {
            printf("Invalid key file!\n");
            return false;
         }
      }

      // Verbosity.
      if (strncasecmp(argv[argc], "-v", 2) == 0) {
         cmdline.mVerbose = true;
      }
   }

   // Validation.
   if (strlen(cmdline.mInterface) == 0) {
      printf("No network interface specified!\n");
      return false;
   }
   if (strlen(cmdline.mKeyFile) == 0) {
      printf("No key file specified!\n");
      return false;
   }
 
   // Validation success.
   return true;
}

// Show Interfaces available in the system.
void showInterfaces()
{
   pcap_if_t* pIn = 0;
   char err[PCAP_ERRBUF_SIZE];

   // Get all devices...
   pcap_findalldevs(&pIn, err);
   pcap_if_t* pCur = pIn;
   printf("Interfaces available:\n");
   while (pCur) {
      printf("%s\n", pCur->name);
      pCur = pCur->next;
   }

   pcap_freealldevs(pIn);
}

void getInterfaceIP(const char* const name, char* ip, uint16_t ipMaxLen)
{
   pcap_if_t* pIn = 0;
   char err[PCAP_ERRBUF_SIZE];

   memset(ip, 0, ipMaxLen);

   // Get all devices...
   pcap_findalldevs(&pIn, err);
   pcap_if_t* pCur = pIn;
   while (pCur) {
      if (strcmp(name, pCur->name) != 0) {
         // Next interface...
         pCur = pCur->next;
         continue;
      }

      // Found interface.
      pcap_addr_t* pAddr = pCur->addresses;
      while (pAddr) {
         // Find address.
         if (pAddr->addr->sa_family != AF_INET) {
            pAddr = pAddr->next;
            continue;
         }

         // Address found!
         strncpy(ip,
            inet_ntoa(((struct sockaddr_in*)pAddr->addr)->sin_addr),
            ipMaxLen
         );
   
         pcap_freealldevs(pIn);
         return;
      }
   }

   // Done.
   pcap_freealldevs(pIn);
}

// Process command line parameters (overarching function).
bool processCommandline(int argc, char** argv)
{
   // Process command line parameters.
   if (!getCmdLineOpts(argc, argv, gSniff.mCmdline)) {
      usage();

      // Do we need to show the interfaces?
      if (gSniff.mCmdline.mShowInterfaces)
         showInterfaces();

      return false;
   }

   // Show interfaces.
   if (gSniff.mCmdline.mShowInterfaces) {
      showInterfaces();
   }

   // Get the device IP.
   getInterfaceIP(gSniff.mCmdline.mInterface, gSniff.msd.mLocalIP, 16);
   if (strlen(gSniff.msd.mLocalIP) == 0) {
      return false;
   }

   // Proceed.
   showCmdline(&gSniff);

   return true;
}

//
// KEYWORD PROCESSING
// For simplicity, I adopted a simple single directional linked list.
// This was something quick and wanted to come up with an efficient yet compact
// and non wasteful way of storing data. Could have used STL as well but my
// preference is to stick to a lower layer due to efficiency considerations.
bool loadKeywords(Sniff& s)
{
   // First open the file. This should work as per previous test on init.
   FILE* f = fopen(s.mCmdline.mKeyFile, "rb");
   if (!f) {
      logErr(s.mLog, logminimal, "could not open key file!");
      return false;
   }

   // For quickness, the best thing to do is just alloc the buffer in memory.
   // This is not the preferred way of doing things but we also are not
   // expecting a lot of data to be in the key file. If there is an attempt
   // dos attack with an enormous file, this will fail and exit.
   fseek(f, 0, SEEK_END);
   long fileSize = ftell(f);
   // 8Mb keyfile is way too much but lets just allow for that under the
   // assumption that resources are available.
   if (fileSize <= 0 || fileSize > 1024 * 1024 * 8) {
      logErr(s.mLog, logminimal, "invalid key file size!");
      fclose(f);
      return false;
   }
   fseek(f, 0, SEEK_SET);

   // Alloc a buffer and copy from file.
   char* pData = (char*)malloc(fileSize + 1);
   if (!pData) {
      logCri(s.mLog, logminimal, "no memory!");
      fclose(f);
      return false;
   }

   // Read from file.
   char* pRead = pData;
   long remaining = fileSize;
   memset(pData, 0, fileSize + 1);
   while (remaining > 0) {
      // Read bytes.
      int bytes = fread(pRead, 1, 10240, f);
      remaining -= bytes;
      pRead += bytes;

      // Just in case there is an error.
      if (ferror(f)) {
         logErr(s.mLog, logminimal, "error io.");
         fclose(f);
         free(pData);
         return false;
      }
   }

   // File read.
   fclose(f);

   // Replace \n with \0 and update keywords.
   Keywords* pCurrent = &s.msd.mKeywords;
   char* pString = pData;
   while (pCurrent) {
      // Store current.
      pCurrent->mpKeyword = pString;
      pCurrent->mpNext = 0;

      // Is there another one?
      pString = strchr(pString, '\n');
      if (pString) {
         *pString = 0;
         pString++;

         // Create next keyword.
         pCurrent->mpNext = (Keywords*)malloc(sizeof(Keywords));
         if (!pCurrent->mpNext) {
            logWarn(s.mLog, logminimal, "mem: not all keywords were loaded");
            return true;      // we still have a valid list for now...
         }
         memset(pCurrent->mpNext, 0, sizeof(Keywords));
      }

      // Next.
      if (gSniff.mCmdline.mVerbose)
         printf("found keyword: %s\n", pCurrent->mpKeyword);
      pCurrent = pCurrent->mpNext;
   }

   // Close.
   return true;
}

void freeKeywords(Sniff& s)
{
   // Free all keywords structs first.
   Keywords* pCur = s.msd.mKeywords.mpNext;
   while (pCur) {
      Keywords** ppPrev = &pCur;
      pCur = (*ppPrev)->mpNext;

      free(*ppPrev);
      *ppPrev = 0;
   }

   // Free individual block of memory.
   if (s.msd.mKeywords.mpKeyword != 0) {
      free(s.msd.mKeywords.mpKeyword);
      s.msd.mKeywords.mpKeyword = 0;
   }
}

//
// CAPTURE DEVICE
//

bool initDevice(Sniff& s)
{
   char buf[2048];

   // Open interface.
   s.msd.mpcap = pcap_open_live(s.mCmdline.mInterface,
      2048, 0, 0, s.msd.mError);
   if (!s.msd.mpcap) {
      printf("open() fail\n");
      printf("   %s\n", s.msd.mError);
      return false;
   }

   // Done.
   return true;
}

void termDevice(Sniff& s)
{
   pcap_close(s.msd.mpcap);

   memset(&s, 0, sizeof(Sniff));
}

//
// CAPTURE PROCESS (IP LEVEL)
//

void cbProcessPacket(Sniff* pSniff)
{
   Packet* pkt = &pSniff->msd.mCaptured;

   // I used this reference:
   // https://vichargrave.github.io/programming/
   //    develop-a-packet-sniffer-with-libpcap/#determine-the-datalink-type
   // I don't know how these dll packet sizes come from.
   int dlType = pcap_datalink(pSniff->msd.mpcap);
   int dlSize = 0;
   switch (dlType) {
   case DLT_EN10MB:
      dlSize = 14;
      break;
   default:
      printf("Unsupported data link layer (%d)\n", dlType);
      return;
   }

   // Get IP Header following data link header.
   const char* pNetData = pkt->mpBytes + dlSize;
   struct ip* phdrIP = (struct ip*)pNetData;
   pSniff->msd.mCaptured.mIPSize = ntohs(phdrIP->ip_len);

   // Determine TCP/UDP/ICMP...
   switch (phdrIP->ip_p) {
   case IPPROTO_TCP:
      cbProcessTCP(pSniff, phdrIP);
      return;
   default:
      // Unsupported protocol.
      return;
   }
}

// Call back to the capture loop - packet capturing...
void cbCapture(u_char* user, const pcap_pkthdr* hdr, const u_char* pBytes)
{
   if (!user) return;
   Sniff* pSniff = (Sniff*)user;

   // Do not support multiple threads (not sure if it ever happens here...).
   pthread_mutex_lock(&pSniff->mLock);

   // Note:
   // In the capture loop, I was expecting that in the header, caplen == len.
   // One would expect, that when an interface works like this, it allows for
   // the potential that caplen < len. In allowing for that, the original
   // implementation of this capture section allowed for first getting the
   // first part, and subsequently, until caplen == len, getting the remaining
   // parts.
   // This did not work as every time a partial sized capture was made, the
   // next part was not available.
   // As a result, any caplen < len captures made have been discarded.
   if (hdr->caplen < hdr->len) {
      pthread_mutex_unlock(&pSniff->mLock);
      return;
   }

   // Process.
   pSniff->msd.mCaptured.mpBytes = (const char*) pBytes;
   pSniff->msd.mCaptured.mPktHdr = hdr;

   // Process packet.
   cbProcessPacket(pSniff);
   fflush(stdout);

   // Do not support multiple threads (not sure if it ever happens here...).
   pthread_mutex_unlock(&pSniff->mLock);
}

bool capture(Sniff& s)
{
   // Default assertions - should not fail here.
   if (s.mCmdline.mInterface[0] == 0) return false;
   if (!s.msd.mpcap) return false;

   // Capture forever for now...
   pcap_loop(s.msd.mpcap, -1, cbCapture, (u_char*)&s);
   return true;
}

//
// TCP PACKET PROCESSING UPPER LAYER BUFFERS
// TCP Packets prefix the application headers.
// They are prefixed by the IP header.
// The TCP layer does not identify with the application
// layer but we can try assess when an application needs to
// process it's buffer when the direction of transfer
// changes.
// 

// Logging
void showIPTCP(Sniff* pSniff, struct ip* pIP, tcphdr* pTCP,
   ULBuffer* pUL, bool hexDump) 
{
   // IP Header comes first...
   short indent = 0;
   if (pIP) {
      uint16_t frag = ntohs(pIP->ip_off);
      bool d = ((frag & 0b01000000000000000) == 0b01000000000000000);
      bool m = ((frag & 0b00100000000000000) == 0b00100000000000000);
      uint16_t fo = (frag & 0b0001111111111111);

      char ipSrc[16];
      char ipDst[16];
      memset(ipSrc, 0, 16);
      memset(ipDst, 0, 16);
      strncpy(ipSrc, inet_ntoa(pIP->ip_src), 16);
      strncpy(ipDst, inet_ntoa(pIP->ip_dst), 16);

      // IP Header.
      logInfo(pSniff->mLog, logmore,
         "IP: bytes=%d(0x%04x), [%c%c], fragoff = %d, %s ==> %s",
         ntohs(pIP->ip_len), ntohs(pIP->ip_len),
         d ? 'd' : '.', m ? 'm' : '.', fo,
         ipSrc, ipDst
      );
      logindent(pSniff->mLog);
      indent++;
   }

   // TCP Header.
   if (pTCP) {
      logInfo(pSniff->mLog, logmore,
         "TCP: seq: %u, [%c%c%c%c%c%c], srcport: %d, dstport: %d",
         ntohl(pTCP->seq),
         pTCP->urg ? 'u' : '.', pTCP->ack ? 'a' : '.', pTCP->psh ? 'p' : '.',
         pTCP->rst ? 'r' : '.', pTCP->syn ? 's' : '.', pTCP->fin ? 'f' : '.',
         htons(pTCP->source), htons(pTCP->dest)
      );
      logindent(pSniff->mLog);
      indent++;
   }

   // Upper layer data...
   if (pUL) {
      // Show endpoints first...
      if (pUL->mpId) {
         char src[16];
         char dst[16];
         in_addr asrc; asrc.s_addr = pUL->mpId->mIPSource;
         in_addr adst; adst.s_addr = pUL->mpId->mIPDest;
         strncpy(src, inet_ntoa(asrc), 16);
         strncpy(dst, inet_ntoa(adst), 16);

         logInfo(pSniff->mLog, logmore,
            "UL: %s:%d ==> %s:%d, dir: %s",
            src, ntohs(pUL->mpId->mPortSource),
            dst, ntohs(pUL->mpId->mPortDest),
            pUL->mpId->mInverseDir ? "inv" : "std"
         );

         logindent(pSniff->mLog); indent++;
      }

      // Dump the layer buffer.
      if (hexDump && pUL->mpBuf && pUL->mSize > 0) {
         logHex(pSniff->mLog, logmore, 32,
            pUL->mpBuf, pUL->mSize, "upperlayer");
      }
   }

   while (indent > 0) {
      logoutdent(pSniff->mLog);
      indent--;
   }
}

// Endpoints
// Endpoints define a source and destination. They are there to identify 
// the source and destination of a packet and are used in upper layer buffers
// ULBuffer to identify when a transmission has stopped. When the source
// and destination get swapped, this mechanism is used to transfer parsing
// to the upper layer protocols.

// Get endpoint or create a new one if it does not exist.
// Swap it's direction if need be.
Endpoints* getEndpoints(Endpoints* pRoot,
   uint32_t ipsrc, uint16_t psrc,
   uint32_t ipdst, uint16_t pdst,
   bool& hasSwapped)
{
   Endpoints* pCur = pRoot;
   Endpoints* pLast = pRoot;
   hasSwapped = false;

   while (pCur) {
      // Source and destination match?
      bool srcok = (pCur->mIPSource == ipsrc && pCur->mPortSource == psrc);
      bool dstok = (pCur->mIPDest == ipdst && pCur->mPortDest == pdst);
      if (srcok && dstok) {
         if (pCur->mInverseDir) {
            hasSwapped = true;
            pCur->mInverseDir = !pCur->mInverseDir;
         }

         return pCur;
      }

      // Invert match?
      srcok = (pCur->mIPSource == ipdst && pCur->mPortSource == pdst);
      dstok = (pCur->mIPDest == ipsrc && pCur->mPortDest == psrc);
      if (srcok && dstok) {
         if (!pCur->mInverseDir) {
            hasSwapped = true;
            pCur->mInverseDir = !pCur->mInverseDir;
         }

         return pCur;
      }

      // Blank item??
      bool blank = (pCur &&
         pCur->mpNext == 0 &&
         pCur->mIPSource == 0 && pCur->mPortSource == 0 &&
         pCur->mIPDest == 0 && pCur->mPortDest == 0
      );
      if (blank) {
         pCur->mIPSource = ipsrc; pCur->mPortSource = psrc;
         pCur->mIPDest = ipdst; pCur->mPortDest = pdst;
         pCur->mInverseDir = false;
         return pCur;
      }

      // No match!
      pLast = pCur;
      pCur = pCur->mpNext;
   }

   // If none found, create!!
   pCur = (Endpoints*)malloc(sizeof(Endpoints));
   if (!pCur) return 0;

   // Populate.
   pCur->mIPSource = ipsrc; pCur->mPortSource = psrc;
   pCur->mIPDest = ipdst; pCur->mPortDest = pdst;
   pCur->mInverseDir = false;
   pCur->mpNext = 0;

   // Log.
   logInfo(gSniff.mLog, logmore, "created new endpoints for:");
   logindent(gSniff.mLog);
   showEndpoints(&gSniff, pCur);
   logoutdent(gSniff.mLog);

   // Add to list.
   if (pLast) pLast->mpNext = pCur;

   // Return.
   return pCur;
}

//
// Upper layer buffers
// Upper layer buffers hold buffers of data that are aggregated together
// from multiple TCP Packets. The idea is that we want to try and group
// a whole transmission between sender and receiver, parse and process it
// and then get a whole separate transmission between receiver and sender.
// We learnt that the TCP protocol does not have an easy mechanism to 
// distinguish this and trusts the application layer protocol to allow for
// content length to be a key factor in determining when a transmission
// is complete. This mechanism, aims to aid; not replace, that process.

// Gets the upper layer buffer belonging to the endpoints passed.
// If no upper layer buffer is available, it will be created.
// No memory will be allocated though for the buffer data.
ULBuffer* getULBuffer(Sniff* pSniff, Endpoints* pId)
{
   if (!pSniff) return 0;
   ULBuffer* pItem = &pSniff->mULBufs;

   // Find buffer with id.
   ULBuffer* pLast = 0;
   ULBuffer* pUnused = 0;
   // If no buffer is found, allocate a new one...
   while (pItem) {
      // Found...
      if (pItem->mpId == pId)
         return pItem;

      // Unused item...
      if (!pUnused && pItem->mpId == 0)
         pUnused = pItem;

      // Next!
      pLast = pItem;
      pItem = pItem->mpNext;
   }

   // Item not found but unused buffer found...
   if (pUnused) {
      pUnused->mpId = pId;
      if (pUnused->mpBuf) free(pUnused->mpBuf);
      pUnused->mpBuf = 0;
      pUnused->mSize = 0;
      return pUnused;
   }

   // Not found! Create one!
   pLast->mpNext = (ULBuffer*)malloc(sizeof(ULBuffer));
   if (!pLast->mpNext) return 0;

   // Init new item.
   memset(pLast->mpNext, 0, sizeof(ULBuffer));
   pLast->mpNext->mpId = pId;

   // Done!
   return pLast->mpNext;
} 

// Just extends the buffer by that amount of data...
bool extendULBuf(ULBuffer* pULBuf, const char* const bytes, uint16_t size)
{
   // If size by 0, just succeed.
   if (size == 0) return true;

   // Reallocate buffer to new length!
   char* pData = (char*)realloc((void*)pULBuf->mpBuf, pULBuf->mSize + size);
   if (!pData) {
      return false;
   }

   // Copy the buffer...
   memcpy(pData + pULBuf->mSize, bytes, size);

   // Update buffer.
   pULBuf->mpBuf = pData;
   pULBuf->mSize += size;

   return true;
}

void cbProcessTCP(Sniff* pSniff, struct ip* phdrIP)
{
   // After getting the IP Header, the TCP or UDP header follows.
   // In this case, we are dealing with the TCP protocol...
   Packet* pkt = &pSniff->msd.mCaptured;
   tcphdr* pTCP = (tcphdr*)(((char*)phdrIP) + sizeof(struct ip));

   // Get endpoint information.
   bool bSwapped = false;
   Endpoints* pEnds = getEndpoints(&pSniff->mEnds,
      phdrIP->ip_src.s_addr, pTCP->source,
      phdrIP->ip_dst.s_addr, pTCP->dest,
      bSwapped);
   if (!pEnds) {
      logCri(pSniff->mLog, logminimal, "memory error!");
      return;
   }

   // Get endpoint buffer.
   ULBuffer* pulBuf = getULBuffer(pSniff, pEnds);
   if (!pulBuf) {
      logCri(pSniff->mLog, logminimal, "memory error!");
      return;
   }

   // Reference: The TCP/IP Guide:
   // Data Offset: number of 32 bit words of data in the TCP Header.
   // So anything after that is the next datagram.
   uint16_t bytes = pTCP->doff * 4;

   // Post TCP Header - upper layer (UL) data and size. 
   const char* pULData = ((const char*)pTCP) + bytes;
   // Upper layer protocol data = total - tcp & ip headers.
   uint16_t ulSize = pkt->mIPSize - sizeof(struct ip) - bytes;

   logInfo(pSniff->mLog, logmore, "Processing incoming packet...");
   showIPTCP(pSniff, phdrIP, pTCP, 0, false);
   logindent(pSniff->mLog);

   // Detect direction swap and determine whether an end of transmission
   // can take place...
   if (bSwapped) {
      if (pulBuf->mpBuf && pulBuf->mSize > 0) {
         // Processing active packet.
         logInfo(pSniff->mLog, logmore, "Process/discard transmission.");
         logindent(pSniff->mLog);
         // Process existing buffer.
         showIPTCP(pSniff, 0, 0, pulBuf, true);

         // Quick HTTP it!
         if (!goQuick(pSniff->mqh, &(pulBuf->mpBuf), pulBuf->mSize)) {
            logErr(pSniff->mLog, logmore, "not an HTTP packet.");
         }

         // Do not free the buffer. Let QuickHTTP do that for us.
         // We do empty the upper layer buffer for more data though...
         pulBuf->mpBuf = 0;
         pulBuf->mSize = 0;

         logoutdent(pSniff->mLog);
      }
   }

   // Bytes adding log...
   logInfo(pSniff->mLog, logmore,
      "adding 0x%04x bytes to 0x%04x byte buffer [0x%08x]...",
      ulSize, pulBuf->mSize, pulBuf
   );
   logoutdent(pSniff->mLog);

   // Update buffer with more data...
   if (!extendULBuf(pulBuf, pULData, ulSize)) {
      logCri(pSniff->mLog, logminimal, "memory error!");
      return;
   }
}

// main().
int main(int argc, char** argv) 
{
   memset(&gSniff, 0, sizeof(Sniff));
   gSniff.mLock = PTHREAD_MUTEX_INITIALIZER;

   // Process command line.
   if (!processCommandline(argc, argv)) return 1;
   
   // Create a logger. This really can't fail but no crashes if it does.
   if (gSniff.mCmdline.mVerbose) {
      gSniff.mLog = createLoggerHandle(0, logmore, 1); 
   } else {
      gSniff.mLog = createLoggerHandle(0, logminimal, 1);
   }
   gSniff.mqh.mLog = gSniff.mLog;

   // Open device.
   if (!initDevice(gSniff))
      return 1;

   // Load up keywords.
   if (!loadKeywords(gSniff)) return 1;

   capture(gSniff);

   // Shutdown.
   termDevice(gSniff);
   freeKeywords(gSniff);

   // Done.
   destroyLoggerHandle(&gSniff.mLog);
   return 0;
}

