// Date:    10th February 2019
// Purpose: Implements a basic network data transmission module.
//
// Version control
// 10 Feb 2019 Duncan Camilleri           Initial development
// 24 Feb 2019 Duncan Camilleri           operators << and >> to int and netnode
//

// Includes
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>                        // netnode
#include <string>
#include <helpers.h>
#include <netaddress.h>                   // netnode
#include <netnode.h>                      // netnode
#include <cycbuf.h>
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
   return mBuf.getReadHead(size);
}

void netdataraw::clearRecvBuf(size_t size)
{
   mBuf.pushReadHead(size);
}

byte* netdataraw::getSendBuf(size_t& size)
{
   return mBuf.getWriteTail(size);
}

void netdataraw::clearSendBuf(size_t size)
{
   mBuf.pushWriteTail(size);
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
bool netdataraw::send()
{
   // First get a buffer to send.
   size_t remaining = 0;
   size_t size = 0;
   char* pBuf = (char*)mBuf.getReadHead(size);
   if (!pBuf || size == 0) return true;

   // A buffer exists for sending.
   remaining = size;
   ssize_t ret = 0;
   while (remaining > 0) {
      // Send buffer of data over the wire.
      ret = ::send(mSocket, pBuf, remaining, 0);
      if (-1 == ret) return false;

      remaining -= ret;
      pBuf += ret;
   }

   // Buffer sent - update cyclic buffer with status about read data.
   // Also; recurse to ensure any buffer at the beginning of the cycle
   // is also processed.
   mBuf.pushReadHead(size);
   return send();
}

bool netdataraw::recv()
{
   // First get output buffer.
   size_t remaining = 0;
   size_t size = 0;
   char* pBuf = (char*)mBuf.getWriteTail(size);
   if (!pBuf || size == 0) return true;

   // A buffer exists for receiving.
   remaining = size;
   ssize_t ret = 0;
   while (remaining > 0) {
      // Receive buffer of data.
      ret = ::recv(mSocket, pBuf, remaining, 0);
      if (-1 == ret) return false;

      remaining -= ret;
      pBuf += ret;
   }

   // Buffer received - update cyclic buffer with status about written data.
   // Also; recurse to ensure any more space available in the buffer is used.
   mBuf.pushWriteTail(size);
   return recv();
}
