/*
Date: 05 Apr 2019 15:20:59.846947626
File: ethframe.h

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

*/

#ifndef __ETHFRAME_H_7CA788C8F0CB1250C9AF79808380E571__
#define __ETHFRAME_H_7CA788C8F0CB1250C9AF79808380E571__

// Check for missing includes.
#ifndef _GLIBCXX_STRING
#error "server.h: missing include - string"
#endif

#define ETHFRAME_MAXDATA               1500
#define ETHFRAME_MAXSIZE               1518

// Determine protocol string based on protocol id hproto.
void ethProto(uint16_t hproto, std::string& out);

#endif   // __ETHFRAME_H_7CA788C8F0CB1250C9AF79808380E571__

