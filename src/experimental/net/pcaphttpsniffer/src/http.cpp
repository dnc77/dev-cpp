/*
Date: 14 May 2021 13:35:14.979717366
File: http.cpp

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

Purpose:

Version control
14 May 2021 Duncan Camilleri           Initial development
*/

extern "C" {
   #include "logger.h"
}

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <memory.h>
#include <pcap.h>
#include <zlib.h>
#include "http.h"
#include "structs.h"

//
// GENERICS
//
#define min(a, x)          (a <= x ? a : x)

//
// ZLIB
// Let's keep this quick for now... 
//

// Needs a good revision...
uint32_t gunzip(const char* pSrc, uint32_t nSrcSize, char** ppOut)
{
   // Initial validations...
   if (!pSrc || nSrcSize <= 0) return 0;
   if (!ppOut) return 0;
   if (*ppOut) free(*ppOut);
   *ppOut = 0;

   const int knOutChunk = 128 * 1024;

   // Output buffer allocation.
   char* pOut = (char*)malloc(knOutChunk);
   if (!pOut) {
      return 0;
   }

   // Initialize output buffer and information.
   memset(pOut, 0, knOutChunk);
   char* pOutPtr = pOut;

   // Init zlib.
   int ret;
   z_stream strm;
   strm.zalloc = Z_NULL;
   strm.zfree = Z_NULL;
   strm.opaque = Z_NULL;
   strm.avail_in = nSrcSize;
   strm.next_in = (Bytef*)pSrc;
   strm.avail_out = knOutChunk;
   strm.next_out = (Bytef*)pOut;
   // ret = inflateInit2(&strm, 16 + MAX_WBITS);
   ret = inflateInit2(&strm, 15 | 32);
   // ret = inflateInit(&strm);
   if (ret != Z_OK) {
      free(pOut);
      return 0;
   }

   // Input buffer.
   const char* pInPtr = pSrc;
   do {
      strm.avail_in = nSrcSize - strm.total_in;
      strm.next_in = (Bytef*)pInPtr + strm.total_in;

      // Inflate current until exhaustion.
      do {
         // Do we need more memory?
         if (strm.avail_out == 0) {
            char* pOrigOut = pOut;
            pOut = (char*)realloc(pOut, strm.total_out + knOutChunk);
            if (!pOut) {
               free(pOrigOut);
               inflateEnd(&strm);
               return 0;
            }
            pOutPtr = pOut + strm.total_out;
         
            // Update zstream.
            strm.avail_out = strm.total_out + knOutChunk;
            strm.next_out = (Bytef*)pOutPtr;
         }
         ret = inflate(&strm, Z_NO_FLUSH);
         // ret = inflate(&strm, Z_FULL_FLUSH);
         assert(ret != Z_STREAM_ERROR);
         switch (ret) {
         case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
         case Z_DATA_ERROR:
         case Z_MEM_ERROR:
            free(pOut);
            (void)inflateEnd(&strm);
            return 0;
         }

      } while (strm.avail_out == 0);
   } while (ret != Z_STREAM_END);

   // Done.
   inflateEnd(&strm);
   if (ret != Z_STREAM_END) {
      free(pOut);
      return 0;
   }   

   // Return allocated size after assigning to ptr.
   uint32_t size = strm.total_out;
   *ppOut = pOut;
   return size;
}

//
// REQUEST PROCESSING
//

// Request:
// 5 Request
// Request       = Request-Line              ; Section 5.1
//                 *(( general-header        ; Section 4.5
//                 | request-header         ; Section 5.3
//                 | entity-header ) CRLF)  ; Section 7.1
//                 CRLF
//                 [ message-body ]          ; Section 4.3

bool processRequest(QuickHTTP& qh)
{
   QuickRq rq;
   memset(&rq, 0, sizeof(QuickRq));

   // Determine method: For our purposes, we are going to detect
   // basic methods GET, POST, PUT, HEAD.
   // Extendable in the future of course.
   if (strncmp(qh.mpData, "GET", 3) == 0) {
      // GET request detected.
      strncpy(rq.mMethod, qh.mpData, 3);
   } else if (strncmp(qh.mpData, "POST", 4) == 0) {
      // POST request detected.
      strncpy(rq.mMethod, qh.mpData, 4);
   } else if (strncmp(qh.mpData, "PUT", 3) == 0) {
      // PUT request detected.
      strncpy(rq.mMethod, qh.mpData, 3);
   } else if (strncmp(qh.mpData, "HEAD", 4) == 0) {
      // HEAD request detected.
      strncpy(rq.mMethod, qh.mpData, 4);
   } else {
      // Not a request.
      return false;
   }

   // Determine URI.
   const char* pCurrent = qh.mpData + strlen(rq.mMethod) + 1;
   const char* const uriEnd = strchr(pCurrent, ' ');
   if (!uriEnd) {
      logErr(qh.mLog, logmore, "httprq -> invalid HTTP request!");
      return false;
   }
   strncpy(rq.mURI, pCurrent, min(uriEnd - pCurrent, 511));

   // HTTP Version.
   pCurrent += uriEnd - pCurrent;
   pCurrent = strchr(pCurrent, ' ') + 1;
   const char* const versionEnd = strchr(pCurrent, '\r');
   strncpy(rq.mHTTPVersion, pCurrent, min(versionEnd - pCurrent, 15));

   // Look up for host (should be present in HTTP1.1.
   const char* pHost = strstr(pCurrent, "Host");
   if (pHost) {
      // Host:_
      pHost += 6;
      const char* const pHostEnd = strchr(pHost, '\r');
      if (pHostEnd) {
         strncpy(rq.mHost, pHost, min(pHostEnd - pHost, 511));
      }
   }
   
   // Log.
   logInfo(qh.mLog, logmore, "httprq -> method %s, uri: %s%s, version: %s",
      rq.mMethod, rq.mHost, rq.mURI, rq.mHTTPVersion
   );

   // Look up and report keywords.
   char onceMsg[2048];
   memset(onceMsg, 0, 2048);
   snprintf(onceMsg, 2048, "httprq -> method %s, uri: %s%s, version: %s\n",
      rq.mMethod, rq.mHost, rq.mURI, rq.mHTTPVersion
   );
   findKeywords(qh, qh.mpData, qh.mSize, onceMsg);

   // Request processed.
   return true;
}

//
// RESPONSE PROCESSING
//

// From the packet data, get the content.
// If the content is gzipped, gunzip it, else copy.
// Packet in response structure will be freed/allocated here.
bool allocResponseData(QuickHTTP& qh, QuickRs& rs)
{
   // First locate end of response headers (with \r\n\r\n).
   const char* const pHeaders = qh.mpData;
   const char* const pHeadersEnd = strstr(pHeaders, "\r\n\r\n") + 4;
   if (!pHeadersEnd) {
      logErr(qh.mLog, logmore, "httprs <- invalid HTTP response!");
      return false;
   }

   // Create buffer pointers to point to packet data first.
   const char* pEncoded = pHeadersEnd;
   int nEncodedSize = qh.mSize - (pHeadersEnd - pHeaders);

   // Delete previous HTML data.
   if (rs.mHTML) free(rs.mHTML);
   rs.mHTML = 0;
   rs.mHTMLSize = 0;

   // Does the data need to be decompressed?
   if (strncmp(rs.mEncoding, "gzip", 4) == 0) {
      // For now we are only supporting gzip...
      rs.mHTMLSize = gunzip(pEncoded, nEncodedSize, &rs.mHTML);
      if (rs.mHTMLSize <= 0) {
         logErr(qh.mLog, logmore, "httprs <- decode failure.");
      }

      // Buffer fetched.
      logHex(qh.mLog, logmore, 32, rs.mHTML, rs.mHTMLSize, "HTML");
   } else if (strncmp(rs.mEncoding, "identity", 8) == 0) {
      // Just copy.
      rs.mHTML = (char*)malloc(nEncodedSize);
      if (!rs.mHTML) {
         logCri(qh.mLog, logminimal, "out of memory!");
         return false;
      }

      // Copy to allocated buffer.
      memcpy(rs.mHTML, pEncoded, nEncodedSize);
      rs.mHTMLSize = nEncodedSize;
   } else {
      logErr(qh.mLog, logmore, "httprs <- unsupported content encoding.");
      return false;
   }

   return true;
}


bool processResponse(QuickHTTP& qh)
{
   QuickRs rs;
   memset(&rs, 0, sizeof(QuickRs));

   // Get HTTP Version first...
   const char* pCurrent = qh.mpData;
   const char* pEnd = strchr(pCurrent, ' ');
   if (!pEnd) {
      logErr(qh.mLog, logmore, "httprs <- invalid HTTP response!");
      return false;
   } else {
      // Copy version and move current pointer to get the status code..
      strncpy(rs.mHTTPVersion, pCurrent, min(15, pEnd - pCurrent));
      pCurrent = pEnd + 1;
   }

   // Get a status code (of course, here we will be more convinced about the
   // validity of an HTTP response packet.
   char statusCode[4];
   memset(statusCode, 0, 4);
   strncpy(statusCode, pCurrent, 3);
   rs.mStatusCode = atoi(statusCode);
   if (rs.mStatusCode == 0) {
      // Not an HTTP response header.
      return false;
   }

   // Note about response codes supported:
   // For the purpose of this exercise, only response code 200 is supported
   // and processed. Other response codes including partial content etc...
   // can be implemented with a few minor modifications when necessary.
   if (rs.mStatusCode != 200) {
      logWarn(qh.mLog, logmore,
         "httprs <- unsupported status code %d", rs.mStatusCode
      );

      // It still was a valid response packet.
      return true;
   }

   // Got a 200 response. Process response.
   // Determine content encoding first.
   const char* pEncoding = strcasestr(pCurrent, "Content-Encoding");
   if (!pEncoding) {
      strncpy(rs.mEncoding, "identity", 63);
   } else {
      pEnd = strchr(pEncoding, '\r');
      if (!pEnd) {
         logErr(qh.mLog, logmore, "Unexpected response format!");

         // Fail.
         return false;
      }

      // Get encoding string.
      pEncoding += 18;
      strncpy(rs.mEncoding, pEncoding, min(pEnd - pEncoding, 63));
   }

   // Encoding section.
   if (!allocResponseData(qh, rs)) return false;

   // Log.
   logInfo(qh.mLog, logmore, "httprs <- http %s, status: %d, encoding: %s",
      rs.mHTTPVersion, rs.mStatusCode, rs.mEncoding
   );

   // Look up and report keywords.
   char onceMsg[2048];
   memset(onceMsg, 0, 2048);
   snprintf(onceMsg, 2048, "httprs <- http %s, status: %d, encoding: %s",
      rs.mHTTPVersion, rs.mStatusCode, rs.mEncoding
   );
   findKeywords(qh, rs.mHTML, rs.mHTMLSize, onceMsg);

   // Free response data.
   free(rs.mHTML);
   rs.mHTML = 0;
   rs.mHTMLSize = 0;
   

   // Response processed.
   return true;
}

// Locates each keyword in the buffer.
void findKeywords(QuickHTTP& qh,
   const char* const pBuf, uint32_t size, char* onceMessage)
{
   // Go through each keyword and if any are found, report...
   Keywords* pCur = qh.mpKeywords;
   bool once = false;
   while (pCur) {
      void* pKeyword = memmem(pBuf, size,
         pCur->mpKeyword, strlen(pCur->mpKeyword)
      );
      if (pKeyword) {
         if (!once) {
            once = true;
            logWarn(qh.mLog, logminimal, onceMessage);
         }

         // Log word.
         logWarn(qh.mLog, logminimal, "Keyword found: %s", pCur->mpKeyword);
      }

      // Next...
      pCur = pCur->mpNext;
   }
}

// Does the buffer end in \r\n\r\n?
bool isBufReady(const char* const pBuf, uint32_t size)
{
   if (!pBuf || size < 4) return false;

   const char* pcEnd = "\r\n\r\n";
   const char* pEnd = &pBuf[size - 4];
   int nCur = 3;
   while (nCur >= 0) {
      if (pEnd[nCur] != pcEnd[nCur]) {
         // Since we found a mismatch, this buffer is not ready.
         return false;
      }

      nCur--;
   }

   // All matching!
   return true;   
}

// goQuick will process the buffer for http.
// It returns false when it fails usually either through no available
// resources or when packet data is not HTTP.
// goQuick will take responsibility of pBuf on calling. Once the caller
// mfer is **always** discarded using free.
bool goQuick(QuickHTTP& qh, char** ppBuf, uint32_t size)
{
   // In this case, goQuick did not fail if it was provided bad params.
   if (!ppBuf) return true;
   char* pBuf = *ppBuf;
   if (!pBuf || size == 0)
      return true;

   // When qh has no buffer, just assign!
   if (!qh.mpData && qh.mSize == 0) {
      qh.mpData = pBuf;
      qh.mSize = size;
      *ppBuf = 0;
   } else {
      // qh must have an incomplete buffer at this point so just extend to it.
      // If we don't have a \r\n\r\n at the end of the current buffer,
      // it cannot be processed. So the provided buffer will need to be
      // appended to the existing one and also freed because we now
      // have responsibility of the buffer.
      char* pData = (char*)realloc((void*)qh.mpData, qh.mSize + size);
      if (!pData) { 
         // If this failed, free up all buffers.
         free(pBuf);
         free((void*)qh.mpData);
         qh.mpData = 0;
         qh.mSize = 0;
         *ppBuf = 0;

         return false;
      }

      // Copy the buffer...
      memcpy(pData + qh.mSize, pBuf, size);
      free(pBuf);
      qh.mpData = pData;
      *ppBuf = 0; 
   }

   // Process buffer if it's ready for processing.
   bool success = true;
   if (isBufReady(qh.mpData, qh.mSize)) {
      if (!processRequest(qh)) {
         success = processResponse(qh);
      }

      // We may have processed a packet or failed.
      // We clean up the buffer and continue.
      // If we don't and the same packet keeps failing, then...
      if (qh.mpData) free((void*)qh.mpData);
      qh.mpData = 0;
      qh.mSize = 0;
      *ppBuf = 0;
   }

   // Advise caller of any serious matters.
   return success;
}

