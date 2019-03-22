/*
Date: 22 Mar 2019 22:39:21.789243130
File: netdataraw.cpp

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

Purpose: Implements a basic network data transmission module.

Version control
10 Feb 2019 Duncan Camilleri           Initial development
24 Feb 2019 Duncan Camilleri           operators << and >> to int and netnode
26 Feb 2019 Duncan Camilleri           clearSendBuf to commitSendBuf
26 Feb 2019 Duncan Camilleri           recv() changed to return immediately
27 Feb 2019 Duncan Camilleri           Introduced send and receive buffers
28 Feb 2019 Duncan Camilleri           Added netdata state ndstate support
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

// Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>                        // netnode
#include <string>
#include <helpers.h>
#include <netaddress.h>                   // netnode
#include <cycbuf.h>                       // netnode
#include <netnode.h>                      // netnode
#include <netdataraw.h>



//
// CONSTRUCTOR/DESCTRUCTOR
//

netdataraw::netdataraw()
{
}

netdataraw::netdataraw(const netdataraw& ndr)
: netdataraw()
{   
}

netdataraw::netdataraw(int socket)
{
   mSocket = socket;
}

netdataraw::~netdataraw()
{
}

//
// DIRECT BUFFER ACCESS
//

byte const* netdataraw::getRecvBuf(size_t& size)
{
   return mRecvBuf.getReadHead(size);
}

void netdataraw::clearRecvBuf(size_t size)
{
   mRecvBuf.pushReadHead(size);
}

byte* netdataraw::getSendBuf(size_t& size)
{
   return mSendBuf.getWriteTail(size);
}

void netdataraw::commitSendBuf(size_t size)
{
   mSendBuf.pushWriteTail(size);
}

//
// SOCKET ASSIGNMENT
//

netdataraw& netdataraw::operator<<(netnode& net)
{
   mSocket = net.mSocket;
}

netdataraw& netdataraw::operator>>(netnode& net)
{
   mSocket = net.mSocket;
}

netdataraw& netdataraw::operator<<(int socket)
{
   mSocket = socket;
}

netdataraw& netdataraw::operator>>(int socket)
{
   mSocket = socket;
}

//
// TRANSMISSION
//

// Will send all the data present in the local cyclic buffer.
// This function will return only when all data has been sent successfully
// with a true. The function will return with a false when an error condition
// during data transfer has occured.
// The state of the cyclic buffer will be emptied after data has been
// successfully sent. In the case of an error, the buffer may have been
// partially sent. The buffer will not be altered however in any situation where
// an error has occurred.
bool netdataraw::send(ndstate& nds)
{
   // First get a buffer to send.
   nds = ndstate::ok;
   size_t remaining = 0;
   size_t size = 0;
   char* pBuf = (char*)mSendBuf.getReadHead(size);
   if (!pBuf || size == 0) return true;

   // A buffer exists for sending.
   remaining = size;
   ssize_t ret = 0;
   while (remaining > 0) {
      // Send buffer of data over the wire.
      ret = ::send(mSocket, pBuf, remaining, 0);
      if (-1 == ret) {
         nds = (errno == ECONNRESET) ? ndstate::disconnected : ndstate::fail;
         return false;
      }

      remaining -= ret;
      pBuf += ret;
   }

   // Buffer sent - update cyclic buffer with status about read data.
   // Also; recurse to ensure any buffer at the beginning of the cycle
   // is also processed.
   mSendBuf.pushReadHead(size);
   return send(nds);
}

// When receiving data, just return as soon as some data is received.
// This could then be processed instantly and freed from the buffer.
// Function returns false when
// - the buffer is full and there is no space to receive more data.
// - when the receive on the socket fails.
// - the socket has disconnected.
bool netdataraw::recv(ndstate& nds)
{
   // First get output buffer.
   nds = ndstate::ok;
   size_t size = 0;
   char* pBuf = (char*)mRecvBuf.getWriteTail(size);
   if (!pBuf || size == 0) return false;

   // A buffer exists for receiving - just receive the next packet.
   size_t ret = ::recv(mSocket, pBuf, size, 0);
   if (-1 == ret) {
      nds = ndstate::fail;
      return false;
   }

   // Disconnected?
   if (0 == ret) {
      nds = ndstate::disconnected;
      return false;
   }

   // If data has been received, then push the write tail so that
   // received data is retained.
   mRecvBuf.pushWriteTail(ret);
   return true;
}
