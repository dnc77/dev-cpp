/*
Date: 22 Mar 2019 22:39:13.791437689
File: cycbuf.h

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

Copyright (C) 2000-2019 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: A cyclic buffer of a defined size.

Version control
05 Feb 2019 Duncan Camilleri           Initial development
10 Feb 2019 Duncan Camilleri           isEmpty() and isFull() are now public
20 Feb 2019 Duncan Camilleri           Renamed pushHead() to pushReadHead()
21 Feb 2019 Duncan Camilleri           Readjusted buffer sizes
21 Feb 2019 Duncan Camilleri           Added copy constructor
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __CYCBUF_H_2A18CFFE1476BE00673E358841C878E9__
#define __CYCBUF_H_2A18CFFE1476BE00673E358841C878E9__

// Check for missing includes.
#if not defined _GLIBCXX_STRING
#error "cycbuf.h: missing include - string"
#elif not defined __HELPERS_H_1181F24416A281704183E457A90E8460__
#error "cycbuf.h: missing include - helpers.h"     // byte definition (no c++17)
#endif

// Different cyclic buffer sizes.
enum cycsiz : unsigned int {
   tiny = 16,
   small = 128,
   medium = 512,
   large = 2048,
   huge = 131072,
   massive = 16777216                              // undefined for now
};

// Cyclic buffer
// Rules:
// * Buffer consists of a start and end defining the space of the whole buffer.
// * Buffer has a head which defines the start of data
// * Buffer has a tail which defines the end of data
// * The tail pointer never contains any valid data
// * The end pointer never contains any valid data
// * When head == tail, the buffer is empty
// * When head == tail - 1, the buffer is full
// * When head == start and tail = end, buffer is also full

template <unsigned int size>
class cycbuf
{
public:
   // Construction/Destruction
   cycbuf();
   cycbuf(const cycbuf& c);
   virtual ~cycbuf();

   // Conversion.
   std::string toString();

   // Copy functions.
   size_t readcopy(byte* pBuf, size_t s);
   size_t writecopy(byte* pBuf, size_t s);

   // Direct access functions.
   // These functions provide direct access to the cyclic buffer
   // memory. Using direct access functions allow for more speed as
   // they reduce the need to copy buffers however their use comes at
   // a cost in that they require more caution.
   // Call getReadHead to get a function pointer to the data in the
   // cyclic buffer and also a size to specify how much data is available.
   // Call pushReadHead and the size of bytes that is no longer necessary. Be
   // wary that the size of bytes to push is within the constraints of the
   // state of the buffer. getReadHead provides the maximum at the time of
   // it's call.
   // Likewise, getWriteTail gives access to a buffer and a size which can be
   // used to write data to. This data will be written to the cyclic buffer.
   // After writing, a call to pushTail with the number of bytes written should
   // be made. Same conditions as the read functions apply in terms of the size.
   byte const* getReadHead(size_t& s);
   void pushReadHead(size_t s);
   byte* getWriteTail(size_t& s);
   void pushWriteTail(size_t s);

private:
   bool mFailWrite = false;               // when the buffer is not big enough

   byte* mpHead = mBuf;                   // head of data
   byte* mpTail = mpHead;                 // tail of data
   byte* mpStart = mBuf;                  // start of buffer
   byte* mpEnd = &mBuf[size - 1];         // end of buffer
   byte mBuf[(int)size];                  // whole buffer

public:
   // Buffer status checks
   bool isEmpty();
   bool isFull();

private:
   bool isReadReady();
   bool isWriteReady();
};

#endif   // __CYCBUF_H_2A18CFFE1476BE00673E358841C878E9__
