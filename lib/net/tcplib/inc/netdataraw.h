// Date:    10th February 2019
// Purpose: Implements a basic network data transmission module.
//
// Version control
// 10 Feb 2019 Duncan Camilleri           Initial development
// 24 Feb 2019 Duncan Camilleri           operators << and >> to int and netnode
//

#ifndef __NETDATARAW_H__
#define __NETDATARAW_H__

// Check for missing includes.
#if not defined __CYCBUF_H__
#error "netxfer.h: missing include - cycbuf.h"
#endif

// netdataraw transmits data over the network without any
// formatting. This is to be used as a base class for any
// form of data transmission.
// For size defined packets of data transmitted over the wire,
// have a look at netdatapkt (to come later).
class netdataraw
{
public:
   // Constructor/destructor
   netdataraw();
   netdataraw(const netdataraw& ndr);
   netdataraw(int socket);
   virtual ~netdataraw();

   // Direct buffer access.
   byte const* getRecvBuf(size_t& size);
   void clearRecvBuf(size_t size);
   byte* getSendBuf(size_t& size);
   void clearSendBuf(size_t size);

   // Socket assignment.
   netdataraw& operator<<(netnode& net);
   netdataraw& operator>>(netnode& net);
   netdataraw& operator<<(int socket); 
   netdataraw& operator>>(int socket);

   // Transmission.
   bool send();
   bool recv();

protected:
   int mSocket = 0;
   cycbuf<medium> mBuf;

private:
};


#endif      // __NETDATARAW_H__

