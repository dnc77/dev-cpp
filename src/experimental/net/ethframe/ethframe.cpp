/*
Date: 05 Apr 2019 15:25:14.243517361
File: ethframe.cpp

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

Purpose:

Version control
05 Apr 2019 Duncan Camilleri           Initial development
09 Apr 2019 Duncan Camilleri           ethProto returns protocol back

*/

#include <arpa/inet.h>        // inet_ntop
#include <linux/if_ether.h>   // ETH_P_ALL
#include <sys/time.h>         // helpers.h
#include <string>
#include <memory.h>
#include <helpers.h>
#include "ethframe.h"

using namespace std;


// Determine protocol string based on protocol id hproto.
uint16_t ethProto(uint16_t hproto, std::string& out)
{
   char tmp[32];
   memset(tmp, 0, 32);
   uint16_t proto = ntohs(hproto);

   switch (proto) {
   case ETH_P_LOOP: sprintf(tmp, "LOOP (0x%04x)", proto); break;
   case ETH_P_PUP: sprintf(tmp, "PUP (0x%04x)", proto); break;
   case ETH_P_PUPAT: sprintf(tmp, "PUPAT (0x%04x)", proto); break;
   case ETH_P_TSN: sprintf(tmp, "TSN (0x%04x)", proto); break;
   case ETH_P_IP: sprintf(tmp, "IP (0x%04x)", proto); break;
   case ETH_P_X25: sprintf(tmp, "X25 (0x%04x)", proto); break;
   case ETH_P_ARP: sprintf(tmp, "ARP (0x%04x)", proto); break;
   case ETH_P_BPQ: sprintf(tmp, "BPQ (0x%04x)", proto); break;
   case ETH_P_IEEEPUP: sprintf(tmp, "IEEEPUP (0x%04x)", proto); break;
   case ETH_P_IEEEPUPAT: sprintf(tmp, "IEEEPUPAT (0x%04x)", proto); break;
   case ETH_P_BATMAN: sprintf(tmp, "BATMAN (0x%04x)", proto); break;
   case ETH_P_DEC: sprintf(tmp, "DEC (0x%04x)", proto); break;
   case ETH_P_DNA_DL: sprintf(tmp, "DNA_DL (0x%04x)", proto); break;
   case ETH_P_DNA_RC: sprintf(tmp, "DNA_RC (0x%04x)", proto); break;
   case ETH_P_DNA_RT: sprintf(tmp, "DNA_RT (0x%04x)", proto); break;
   case ETH_P_LAT: sprintf(tmp, "LAT (0x%04x)", proto); break;
   case ETH_P_DIAG: sprintf(tmp, "DIAG (0x%04x)", proto); break;
   case ETH_P_CUST: sprintf(tmp, "CUST (0x%04x)", proto); break;
   case ETH_P_SCA: sprintf(tmp, "SCA (0x%04x)", proto); break;
   case ETH_P_TEB: sprintf(tmp, "TEB (0x%04x)", proto); break;
   case ETH_P_RARP: sprintf(tmp, "RARP (0x%04x)", proto); break;
   case ETH_P_ATALK: sprintf(tmp, "ATALK (0x%04x)", proto); break;
   case ETH_P_AARP: sprintf(tmp, "AARP (0x%04x)", proto); break;
   case ETH_P_8021Q: sprintf(tmp, "8021Q (0x%04x)", proto); break;
   case ETH_P_IPX: sprintf(tmp, "IPX (0x%04x)", proto); break;
   case ETH_P_IPV6: sprintf(tmp, "IPV6 (0x%04x)", proto); break;
   case ETH_P_PAUSE: sprintf(tmp, "PAUSE (0x%04x)", proto); break;
   case ETH_P_SLOW: sprintf(tmp, "SLOW (0x%04x)", proto); break;
   case ETH_P_WCCP: sprintf(tmp, "WCCP (0x%04x)", proto); break;
   case ETH_P_MPLS_UC: sprintf(tmp, "MPLS_UC (0x%04x)", proto); break;
   case ETH_P_MPLS_MC: sprintf(tmp, "MPLS_MC (0x%04x)", proto); break;
   case ETH_P_ATMMPOA: sprintf(tmp, "ATMMPOA (0x%04x)", proto); break;
   case ETH_P_PPP_DISC: sprintf(tmp, "PPP_DISC (0x%04x)", proto); break;
   case ETH_P_PPP_SES: sprintf(tmp, "PPP_SES (0x%04x)", proto); break;
   case ETH_P_LINK_CTL: sprintf(tmp, "LINK_CTL (0x%04x)", proto); break;
   case ETH_P_ATMFATE: sprintf(tmp, "ATMFATE (0x%04x)", proto); break;
   case ETH_P_PAE: sprintf(tmp, "PAE (0x%04x)", proto); break;
   case ETH_P_AOE: sprintf(tmp, "AOE (0x%04x)", proto); break;
   case ETH_P_8021AD: sprintf(tmp, "8021AD (0x%04x)", proto); break;
   case ETH_P_802_EX1: sprintf(tmp, "802_EX1 (0x%04x)", proto); break;
   case ETH_P_TIPC: sprintf(tmp, "TIPC (0x%04x)", proto); break;
   case ETH_P_MACSEC: sprintf(tmp, "MACSEC (0x%04x)", proto); break;
   case ETH_P_8021AH: sprintf(tmp, "8021AH (0x%04x)", proto); break;
   case ETH_P_MVRP: sprintf(tmp, "MVRP (0x%04x)", proto); break;
   case ETH_P_1588: sprintf(tmp, "1588 (0x%04x)", proto); break;
   case ETH_P_NCSI: sprintf(tmp, "NCSI (0x%04x)", proto); break;
   case ETH_P_PRP: sprintf(tmp, "PRP (0x%04x)", proto); break;
   case ETH_P_FCOE: sprintf(tmp, "FCOE (0x%04x)", proto); break;
   case ETH_P_TDLS: sprintf(tmp, "TDLS (0x%04x)", proto); break;
   case ETH_P_FIP: sprintf(tmp, "FIP (0x%04x)", proto); break;
   case ETH_P_80221: sprintf(tmp, "80221 (0x%04x)", proto); break;
   case ETH_P_HSR: sprintf(tmp, "HSR (0x%04x)", proto); break;
   case ETH_P_LOOPBACK: sprintf(tmp, "LOOPBACK (0x%04x)", proto); break;
   case ETH_P_QINQ1: sprintf(tmp, "QINQ1 (0x%04x)", proto); break;
   case ETH_P_QINQ2: sprintf(tmp, "QINQ2 (0x%04x)", proto); break;
   case ETH_P_QINQ3: sprintf(tmp, "QINQ3 (0x%04x)", proto); break;
   case ETH_P_EDSA: sprintf(tmp, "EDSA (0x%04x)", proto); break;
   case ETH_P_AF_IUCV: sprintf(tmp, "AF_IUCV (0x%04x)", proto); break;
   case ETH_P_802_3_MIN: sprintf(tmp, "802_3_MIN (0x%04x)", proto); break;
   case ETH_P_802_3: sprintf(tmp, "802_3 (0x%04x)", proto); break;
   case ETH_P_AX25: sprintf(tmp, "AX25 (0x%04x)", proto); break;
   case ETH_P_ALL: sprintf(tmp, "ALL (0x%04x)", proto); break;
   case ETH_P_802_2: sprintf(tmp, "802_2 (0x%04x)", proto); break;
   case ETH_P_SNAP: sprintf(tmp, "SNAP (0x%04x)", proto); break;
   case ETH_P_DDCMP: sprintf(tmp, "DDCMP (0x%04x)", proto); break;
   case ETH_P_WAN_PPP: sprintf(tmp, "WAN_PPP (0x%04x)", proto); break;
   case ETH_P_PPP_MP: sprintf(tmp, "PPP_MP (0x%04x)", proto); break;
   case ETH_P_LOCALTALK: sprintf(tmp, "LOCALTALK (0x%04x)", proto); break;
   case ETH_P_CAN: sprintf(tmp, "CAN (0x%04x)", proto); break;
   case ETH_P_CANFD: sprintf(tmp, "CANFD (0x%04x)", proto); break;
   case ETH_P_PPPTALK: sprintf(tmp, "PPPTALK (0x%04x)", proto); break;
   case ETH_P_TR_802_2: sprintf(tmp, "TR_802_2 (0x%04x)", proto); break;
   case ETH_P_MOBITEX: sprintf(tmp, "MOBITEX (0x%04x)", proto); break;
   case ETH_P_CONTROL: sprintf(tmp, "CONTROL (0x%04x)", proto); break;
   case ETH_P_IRDA: sprintf(tmp, "IRDA (0x%04x)", proto); break;
   case ETH_P_ECONET: sprintf(tmp, "ECONET (0x%04x)", proto); break;
   case ETH_P_HDLC: sprintf(tmp, "HDLC (0x%04x)", proto); break;
   case ETH_P_ARCNET: sprintf(tmp, "ARCNET (0x%04x)", proto); break;
   case ETH_P_DSA: sprintf(tmp, "DSA (0x%04x)", proto); break;
   case ETH_P_TRAILER: sprintf(tmp, "TRAILER (0x%04x)", proto); break;
   case ETH_P_PHONET: sprintf(tmp, "PHONET (0x%04x)", proto); break;
   case ETH_P_IEEE802154: sprintf(tmp, "IEEE802154 (0x%04x)", proto); break;
   case ETH_P_CAIF: sprintf(tmp, "CAIF (0x%04x)", proto); break;
   case ETH_P_XDSA: sprintf(tmp, "XDSA (0x%04x)", proto); break;
   default: sprintf(tmp, "unknown (0x%04x)", proto);
   }

   out = tmp;
   return proto;
}

