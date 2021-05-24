/*
Date: 14 May 2021 13:33:23.006779302
File: http.h

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

Copyright (C) 2014 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Version control
14 May 2021 Duncan Camilleri           Initial development
*/

#ifndef __HTTP_H_35D28998F251E195AD03AAD59D92CFDD__
#define __HTTP_H_35D28998F251E195AD03AAD59D92CFDD__


// References:
// https://tools.ietf.org/html/rfc2616
// https://tools.ietf.org/html/rfc7231
// 

// Forward declare keyword.
struct tagKeywords;
typedef struct tagKeywords Keywords;

typedef struct tagQuickHTTP {
   loghdl mLog;
   Keywords* mpKeywords;

   const char* mpData;
   uint16_t mSize;
} QuickHTTP;

typedef struct tagQuickRq {
   char mMethod[8];
   char mHost[512];
   char mURI[512];
   char mHTTPVersion[16];
} QuickRq;

typedef struct tagQuickRs {
   uint16_t mStatusCode;
   char mHTTPVersion[16];
   char mEncoding[64];

   char* mHTML;
   uint32_t mHTMLSize;
} QuickRs;

bool processRequest(QuickHTTP& qh);

bool allocResponseData(QuickHTTP& qh, QuickRs& rs);
bool processResponse(QuickHTTP& qh);

void findKeywords(QuickHTTP& qh,
   const char* const pBuf, uint32_t size, char* onceMessage);
bool isBufReady(const char* const pBuf, uint32_t size);
bool goQuick(QuickHTTP& qh, char** ppBuf, uint32_t size);


#endif   // __HTTP_H_35D28998F251E195AD03AAD59D92CFDD__

