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
         While possible, all sockets assigned to this class should not block.
         This and all child implementations of this class are not designed with
         blocking operations in mind.

Version control
10 Feb 2019 Duncan Camilleri           Initial development
24 Feb 2019 Duncan Camilleri           operators << and >> to int and netnode
26 Feb 2019 Duncan Camilleri           clearSendBuf to commitSendBuf
26 Feb 2019 Duncan Camilleri           recv() changed to return immediately
27 Feb 2019 Duncan Camilleri           Introduced send and receive buffers
28 Feb 2019 Duncan Camilleri           Added netdata state ndstate support
22 Mar 2019 Duncan Camilleri           Added copyright notice
31 Mar 2019 Duncan Camilleri           Use libraries from same repository
02 Apr 2019 Duncan Camilleri           Try cycle buffer when recv() done
02 Apr 2019 Duncan Camilleri           Bug not checking for no data available
02 Apr 2019 Duncan Camilleri           ::recv() returns ssize_t not size_t
02 Apr 2019 Duncan Camilleri           Support for a full buffer
*/

// Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>                        // netnode
#include <string>
#include <helpers.h>
#include <net/netaddress.h>               // netnode
#include <datastruct/cycbuf.h>            // netnode
#include <net/netnode.h>                  // netnode
#include <net/netdataraw.h>



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
   return *this;
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

// Since the intention for this class is to operate on non blocking
// sockets, there is no need to check whether there is data before receiving.
// If the socket were blocking, this would be a problem which can be solved
// by introducing select or poll. Alas; select has a bug where by; on rare
// occasion, it will suggest data is available when there is not. The intention
// of this class is to operate on non-blocking sockets so as soon as recv is
// called, this is done on the cyclic buffer immediately. If the buffer is
// exhausted, an attempt is made to receive again after cycling the buffer.
// When recv finishes, nds state is updated accordingly.
// The received data could be processed by calling getRecvBuf() and released by
// calling clearRecvBuf().
// Function returns false when
// - when the receive on the socket fails.
// - the socket has disconnected.
// Function returns true when
// - data has been received successfully.
// - when the buffer is full (nds = bufferfull).
bool netdataraw::recv(ndstate& nds)
{
   // Initialize.
   nds = ndstate::ok;
   char* pBuf = nullptr;
   size_t size = 0;
   ssize_t recvd = 0;

   // Lambda to receive data. Will return -1 or 0 to suggest the
   // whole operation should return with a failure or success
   // respectively.
   // Will return 1 to suggest that data has been received and the
   // operation should proceed.
   auto recvnow = [&]() -> ssize_t {
       // Get buffer.
      pBuf = (char*)mRecvBuf.getWriteTail(size);
      if (!pBuf || size == 0) {
         nds = ndstate::bufferfull;
         return 0;
      }

      // Receive data to buffer.
      recvd = ::recv(mSocket, pBuf, size, 0);
      if (-1 == recvd) {
         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data to receive (success).
            return 0;
         } else {
            // Operation failed.
            nds = ndstate::fail;
            return -1;
         }
      } else if (0 == recvd) {
         // Disconnected?
         nds = ndstate::disconnected;
         return -1;
      }

      // Data received. Move write tail pointer by the number of bytes
      // received to avoid overwriting.
      mRecvBuf.pushWriteTail(recvd);
      return 1;
   };

   // A buffer exists for receiving - just receive the next packet.
   ssize_t resume = recvnow();
   if (-1 == resume) return false;
   else if (0 == resume) return true;

   // If the buffer has been filled to the end, then try receiving more data
   // after cycling the buffer (if possible).
   if (recvd == size) {             // recvd cannot be greater than size.
      resume = recvnow();
      if (-1 == resume) return false;
      else if (0 == resume) return true;

      // At this point, check to see if the buffer is full.
      // In such a case, there would be more data that needs to be received.
      if (recvd == size) nds = ndstate::bufferfull;
   }

   // All data received.
   return true;
}
