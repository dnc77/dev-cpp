/*
Date: 10 Apr 2019 08:32:23.047222977
File: ethlog.h

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

#ifndef __ETHLOG_H_4A3F44793D6D6F761B6E15B74E53C951__
#define __ETHLOG_H_4A3F44793D6D6F761B6E15B74E53C951__

// Check for missing includes.
#ifndef _LINUX_IF_ETHER_H
#error "ethlog.h: missing include - linux/if_ether.h"
#elif not defined _STDINT_H
#error "ethlog.h: missing include - stdint.h"
#endif

void logEthFrame(ethhdr* p, uint32_t len, uint16_t& protocol);


#endif   // __ETHLOG_H_4A3F44793D6D6F761B6E15B74E53C951__

