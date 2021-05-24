/*
Date: 14 May 2021 13:45:54.218447551
File: structs.h

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
11 May 2021 Duncan Camilleri           Initial development
*/

#ifndef __STRUCTS_H_04EB72F61901294AE85477A7BC024CDE__
#define __STRUCTS_H_04EB72F61901294AE85477A7BC024CDE__

//
// CMD LINE PROCESSING 
//

// Command line parameters.
typedef struct tagCmdLine {
   char mInterface[64];
   bool mShowInterfaces;
   char mKeyFile[256];
   bool mVerbose;
} CmdLine;

//
// KEYWORD PROCESSING
//

// Keywords to filter against.
typedef struct tagKeywords {
   tagKeywords* mpNext;
   char* mpKeyword;
} Keywords;

//
// TCP PACKET PROCESSING UPPER LAYER BUFFERS
//

// Endpoints defines all the different src/dest
// incl. ports and is used to identify each TCP buffer
// accordingly. There are in network order...
typedef struct tagEndpoints {
   tagEndpoints* mpNext;
   uint32_t mIPSource;
   uint16_t mPortSource;
   uint32_t mIPDest;
   uint16_t mPortDest;
   bool mInverseDir;
} Endpoints;

// Upper layer buffers are just groups of memory buffers identified
// by an endpoint.
typedef struct tagULBuf {
   tagULBuf* mpNext;
   Endpoints* mpId;

   // Buffer data.
   char* mpBuf;
   uint16_t mSize;
} ULBuffer;

//
// CAPTURE PROCESS (IP LEVEL)
//

// Ip Packet.
typedef struct tagCapturedPacket {
   // Captured packet data.
   const char* mpBytes;
   const pcap_pkthdr* mPktHdr;

   // Ip header data.
   uint16_t mIPSize;          // size of whole IP packet
} Packet;

// IP Header and capturing information.
typedef struct tagSniffData {
   Keywords mKeywords;
   char mError[PCAP_ERRBUF_SIZE];

   char mLocalIP[16];
   pcap_t* mpcap;
   Packet mCaptured;
} SniffData;

//
// SNIFFER APPLICATION
//

// Overarching sniffer application structure.
typedef struct tagSniff {
   loghdl mLog;
   CmdLine mCmdline;

   // Capturing.
   pthread_mutex_t mLock;
   SniffData msd;
   Endpoints mEnds;
   ULBuffer mULBufs;
   QuickHTTP mqh;
} Sniff;

#endif   // __STRUCTS_H_04EB72F61901294AE85477A7BC024CDE__

