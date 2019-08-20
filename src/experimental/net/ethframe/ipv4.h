/*
Date: 24 Apr 2019 14:45:21.752418397
File: ipv4.h

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

#ifndef __IPV4_H_403E973E220090A78E47A22B47D95A34__
#define __IPV4_H_403E973E220090A78E47A22B47D95A34__

//
// BASIC IPV4 STRUCTURE
//

// IPV4 TOS Reference.
#define ipv4tosPrecedence(p)     (byte)((((short)(*p)) & 0b11100000))
#define ipv4tosDelay(p)          (byte)((((short)(*p)) & 0b00010000))
#define ipv4tosThroughput(p)     (byte)((((short)(*p)) & 0b00001000))
#define ipv4tosReliability(p)    (byte)((((short)(*p)) & 0b00000100))
#define ipv4tosPadding(p)        (byte)((((short)(*p)) & 0b00000011))

// IPV4 Flags.
#define ipv4flagsNoFrag(p)       (byte)((((short)(*p)) & 0b0100000000000000))
#define ipv4flagsMoreFrags(p)    (byte)((((short)(*p)) & 0b0010000000000000))
#define ipv4flagsFragOff(p)      (short)((((short)(*p)) & 0b0001111111111111))

// TOS Structure.
typedef struct _ipv4tos {
   unsigned char precedence : 3;
   unsigned char delay : 1;
   unsigned char throughput : 1;
   unsigned char reliability : 1;
   unsigned char padding1 : 2;
} ipv4tos;

// Flags.
typedef struct _ipv4flags {
   unsigned char reserved : 1;
   unsigned char nofragment : 1;
   unsigned char morefragments : 1;
   unsigned short fragmentoff : 13;
} ipv4flags;

//
// PROTOCOL PROCESSING
//

typedef struct _ipv4protocol {
   char mShortName[16];
   short mProtocol;
} ipv4proto;


// Determine protocol string based on protocol id.
void ip4Proto(uint8_t proto, std::string& out);


#endif   // __IPV4_H_403E973E220090A78E47A22B47D95A34__
