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

*/

#include <assert.h>
#include <stdint.h>                    // int types
#include <memory.h>                    // ethframe.h
#include <sys/time.h>                  // helpers.h
#include <linux/if_ether.h>

#include <string>

#include <helpers.h>                   // ethframe.h
#include "ethlog.h"
#include "ethframe.h"

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

