/*
Date: 22 Mar 2019 22:39:14.256522080
File: netdataraw.h

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
24 Feb 2019 Duncan Camilleri           Operators << and >> to int and netnode
25 Feb 2019 Duncan Camilleri           Minor cosmetic change
26 Feb 2019 Duncan Camilleri           clearSendBuf to commitSendBuf
26 Feb 2019 Duncan Camilleri           recv() changed to return immediately
27 Feb 2019 Duncan Camilleri           Introduced send and receive buffers
28 Feb 2019 Duncan Camilleri           Added netdata state ndstate support
22 Mar 2019 Duncan Camilleri           Added copyright notice
23 Mar 2019 Duncan Camilleri           Cyclic buffer check fix
*/

#ifndef __NETDATARAW_H_70782149FBD14889D4E41E8CA2976B0C__
#define __NETDATARAW_H_70782149FBD14889D4E41E8CA2976B0C__

// Check for missing includes.
#if not defined __CYCBUF_H_F25692AD56E4CE3BBACE97C4F90C99B8__
#error "netdataraw.h: missing include - cycbuf.h"
#endif

// Net data state identifies the state of a socket within a netdata
// structure (given that netdataraw is the main parent of any other
// netdata child).
enum class ndstate {
   ok = 0x00,
   fail = 0x01,
   disconnected = 0x02
};

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
   void commitSendBuf(size_t size);

   // Socket assignment.
   netdataraw& operator<<(netnode& net);
   netdataraw& operator>>(netnode& net);
   netdataraw& operator<<(int socket); 
   netdataraw& operator>>(int socket);

   // Transmission.
   bool send(ndstate& nds);
   bool recv(ndstate& nds);

protected:
   int mSocket = 0;
   cycbuf<medium> mSendBuf;
   cycbuf<medium> mRecvBuf;

private:
};


#endif   // __NETDATARAW_H_70782149FBD14889D4E41E8CA2976B0C__
