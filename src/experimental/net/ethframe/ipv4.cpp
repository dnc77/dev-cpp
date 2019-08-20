/*
Date: 24 Apr 2019 14:45:27.208134908
File: ipv4.cpp

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
24 Apr 2019 Duncan Camilleri           Initial development
*/

#ifndef __IPV4_CPP_49F79F47ED13C88FBB06AB4D0D98552F__
#define __IPV4_CPP_49F79F47ED13C88FBB06AB4D0D98552F__

#include <inttypes.h>         // uint8_t
#include <memory.h>           // memset
#include <string>             // string
#include "ipv4.h"

using namespace std;

// Determine protocol string based on protocol id.
void ip4Proto(uint8_t proto, std::string& out)
{
   char tmp[32];
   memset(tmp, 0, 32);

   switch (proto) {
      case 0: sprintf(tmp, "ip (0x%04x)", proto); break;
      case 1: sprintf(tmp, "icmp (0x%04x)", proto); break;
      case 2: sprintf(tmp, "igmp (0x%04x)", proto); break;
      case 3: sprintf(tmp, "ggp (0x%04x)", proto); break;
      case 4: sprintf(tmp, "ipencap (0x%04x)", proto); break;
      case 5: sprintf(tmp, "st (0x%04x)", proto); break;
      case 6: sprintf(tmp, "tcp (0x%04x)", proto); break;
      case 8: sprintf(tmp, "egp (0x%04x)", proto); break;
      case 9: sprintf(tmp, "igp (0x%04x)", proto); break;
      case 12: sprintf(tmp, "pup (0x%04x)", proto); break;
      case 17: sprintf(tmp, "udp (0x%04x)", proto); break;
      case 20: sprintf(tmp, "hmp (0x%04x)", proto); break;
      case 22: sprintf(tmp, "xns-idp (0x%04x)", proto); break;
      case 27: sprintf(tmp, "rdp (0x%04x)", proto); break;
      case 29: sprintf(tmp, "iso-tp4 (0x%04x)", proto); break;
      case 33: sprintf(tmp, "dccp (0x%04x)", proto); break;
      case 36: sprintf(tmp, "xtp (0x%04x)", proto); break;
      case 37: sprintf(tmp, "ddp (0x%04x)", proto); break;
      case 38: sprintf(tmp, "idpr-cmtp (0x%04x)", proto); break;
      case 45: sprintf(tmp, "idrp (0x%04x)", proto); break;
      case 46: sprintf(tmp, "rsvp (0x%04x)", proto); break;
      case 47: sprintf(tmp, "gre (0x%04x)", proto); break;
      case 50: sprintf(tmp, "esp (0x%04x)", proto); break;
      case 51: sprintf(tmp, "ah (0x%04x)", proto); break;
      case 57: sprintf(tmp, "skip (0x%04x)", proto); break;
      case 73: sprintf(tmp, "rspf (0x%04x)", proto); break;
      case 81: sprintf(tmp, "vmtp (0x%04x)", proto); break;
      case 88: sprintf(tmp, "eigrp (0x%04x)", proto); break;
      case 89: sprintf(tmp, "ospf (0x%04x)", proto); break;
      case 93: sprintf(tmp, "ax.25 (0x%04x)", proto); break;
      case 94: sprintf(tmp, "ipip (0x%04x)", proto); break;
      case 97: sprintf(tmp, "etherip (0x%04x)", proto); break;
      case 98: sprintf(tmp, "encap (0x%04x)", proto); break;
      case 103: sprintf(tmp, "pim (0x%04x)", proto); break;
      case 108: sprintf(tmp, "ipcomp (0x%04x)", proto); break;
      case 112: sprintf(tmp, "vrrp (0x%04x)", proto); break;
      case 115: sprintf(tmp, "l2tp (0x%04x)", proto); break;
      case 124: sprintf(tmp, "isis (0x%04x)", proto); break;
      case 132: sprintf(tmp, "sctp (0x%04x)", proto); break;
      case 133: sprintf(tmp, "fc (0x%04x)", proto); break;
      case 135: sprintf(tmp, "mobility-header (0x%04x)", proto); break;
      case 136: sprintf(tmp, "udplite (0x%04x)", proto); break;
      case 137: sprintf(tmp, "mpls-in-ip (0x%04x)", proto); break;
      case 138: sprintf(tmp, "manet (0x%04x)", proto); break;
      case 139: sprintf(tmp, "hip (0x%04x)", proto); break;
      case 140: sprintf(tmp, "shim6 (0x%04x)", proto); break;
      case 141: sprintf(tmp, "wesp (0x%04x)", proto); break;
      case 142: sprintf(tmp, "rohc (0x%04x)", proto); break;
      default: sprintf(tmp, "unknown (0x%04x)", proto);
   };

   // Done.
   out = tmp;
}

#endif   // __IPV4_CPP_49F79F47ED13C88FBB06AB4D0D98552F__

