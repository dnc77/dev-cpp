/*
Date: 14 May 2021 13:43:58.727273084
File: pcapsniff.h

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


#ifndef __PCAPSNIFF_H_8C33AFC291D9A53F2468702B88EC69E5__
#define __PCAPSNIFF_H_8C33AFC291D9A53F2468702B88EC69E5__

// References:
// https://vichargrave.github.io
// The TCP/IP Guide
//
// Dependencies
// libpcap
// zlib

//
// CMD LINE PROCESSING 
//
void usage();
void showCmdline(CmdLine& cmdline);
bool getCmdLineOpts(int argc, char** argv, CmdLine& cmdline);
void showInterfaces();
void getInterfaceIP(const char* const name, char* ip, uint16_t ipMaxLen);
bool processCommandline(int argc, char** argv);

//
// KEYWORD PROCESSING
//
bool loadKeywords(Sniff& s);
void freeKeywords(Sniff& s);

//
// CAPTURE DEVICE 
//
bool initDevice(Sniff& s);
void termDevice(Sniff& s);

//
// CAPTURE PROCESS (IP LEVEL)
//
void cbProcessPacket(Sniff* pSniff);
void cbCapture(u_char* user, const pcap_pkthdr* hdr, const u_char* pBytes);
bool capture(Sniff& s);

//
// TCP PACKET PROCESSING UPPER LAYER BUFFERS
//

// Logging.
inline char* addrToChar(uint32_t n) {
   in_addr a;
   a.s_addr = n;
   return inet_ntoa(a);
}

void showIPTCP(Sniff* pSniff, struct ip* pIP, tcphdr* pTCP,
   ULBuffer* pUL, bool hexDump); 

//
// Endpoints
// Endpoints define a source and destination. They are there to identify 
// the source and destination of a packet and are used in upper layer buffers
// ULBuffer to identify when a transmission has stopped. When the source
// and destination get swapped, this mechanism is used to transfer parsing
// to the upper layer protocols.

inline void showEndpoints(Sniff* pSniff, Endpoints* pep) {
   char src[16];
   char dst[16];
   strncpy(src, addrToChar(pep->mIPSource), 16);
   strncpy(dst, addrToChar(pep->mIPDest), 16);

   logInfo(pSniff->mLog, logmore,
      "endpoints: [%u]%s:%u ==> [%u]%s:%u, dir:%s",
      pep->mIPSource, src, ntohs(pep->mPortSource),
      pep->mIPDest, dst, ntohs(pep->mPortDest),
      pep->mInverseDir ? "inv" : "std"
   );
}
Endpoints* getEndpoints(Endpoints* pRoot,
   uint32_t ipsrc, uint16_t psrc,
   uint32_t ipdst, uint16_t pdst,
   bool& hasSwapped);

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
ULBuffer* getULBuffer(Sniff* pSniff, Endpoints* pId);

// Upper layer detection
bool extendULBuf(ULBuffer* pULBuf, const char* const bytes, uint16_t size);
void cbProcessTCP(Sniff* pSniff, struct ip* phdrIP);

#endif   // __PCAPSNIFF_H_8C33AFC291D9A53F2468702B88EC69E5__

