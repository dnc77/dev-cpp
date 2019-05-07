/*
Date: 10 Apr 2019 08:32:33.629458660
File: ethlog.cpp

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

Copyright (C) 2000-2025 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Logging functionality for ethernet data structures.

Version control
10 Apr 2019 Duncan Camilleri           Initial development
24 Apr 2019 Duncan Camilleri           Support for IPV4 header
07 May 2019 Duncan Camilleri           IPv4 header logging touch ups

*/

#include <assert.h>
#include <stdint.h>                    // int types
#include <memory.h>                    // ethframe.h
#include <sys/time.h>                  // helpers.h
#include <linux/if_ether.h>            // ethhdr
#include <netinet/ip.h>                // iphdr

#include <string>

#include <helpers.h>                   // ethframe.h
#include "ethlog.h"
#include "ethframe.h"
#include "ipv4.h"

using namespace std;

// Logs an ethernet frame and populates it's protocol.
void logEthFrame(ethhdr* p, uint32_t len, uint16_t& protocol)
{
   assert(p);

   string ethprotocol;
   protocol = ethProto(p->h_proto, ethprotocol);
   printf("ethframe\n");
   printf("dest: %02x:%02x:%02x:%02x:%02x:%02x\n",
      p->h_dest[0], p->h_dest[1], p->h_dest[2],
      p->h_dest[3], p->h_dest[4], p->h_dest[5]);
   printf("source: %02x:%02x:%02x:%02x:%02x:%02x\n",
      p->h_source[0], p->h_source[1], p->h_source[2],
      p->h_source[3], p->h_source[4], p->h_source[5]);
   printf("protocol: %s (%d)\n", ethprotocol.c_str(), protocol);
   printf("length: %d\n", len);
}

// Logs an ipv4 header.
void logIPV4Header(iphdr* p)
{
   assert(p);

   // Type of service.
   uint8_t short tos = ntohs(p->tos);
   uint8_t flags = ntohs(p->frag_off);
   uint8_t totalLen = ntohs(p->tot_len);
   uint8_t id = ntohs(p->id);
   ipv4tos* pTos = (ipv4tos*)(&tos);
   ipv4flags* pFlags = (ipv4flags*)(&flags);

   printf("ipheader v4\n");
   printf("version: %d\n", p->version);
   printf("header length: %d bytes\n", p->ihl * 4);
   printf("tos: prec: %03d del: %01d tput: %01d rel: %01d\n",
      ipv4tosPrecedence((short*)pTos), ipv4tosDelay((short*)pTos),
      ipv4tosThroughput((short*)pTos), ipv4tosReliability((short*)pTos)
   );
   printf("total length: %d\n", totalLen);
   printf("identification: 0x%04x\n", id);
   printf("flags: nofrag: %01d morefrags: %01d: fragoff: %02d\n",
      ipv4flagsNoFrag((short*)pFlags), ipv4flagsMoreFrags((short*)pFlags),
      ipv4flagsFragOff((short*)pFlags)
   );
   printf("ttl: %03d\n", p->ttl);

   // Protocol.
   string protocol;
   ip4Proto(p->protocol, protocol);
   printf("protocol: %s\n", protocol.c_str());
   printf("header checksum: 0x%02x\n", p->check);

   // Addresses.
   unsigned char* pAddr = (unsigned char*)&p->saddr;
   printf("source ip: %03d.%03d.%03d.%03d\n",
      pAddr[0], pAddr[1], pAddr[2], pAddr[3]
   );

   pAddr = (unsigned char*)&p->daddr;
   printf("destination ip: %03d.%03d.%03d.%03d\n",
      pAddr[0], pAddr[1], pAddr[2], pAddr[3]
   );

   // Options - left out for the time being.

   // Padding - nothing to log.

   // Protocol data - nothing to log for now.
}

