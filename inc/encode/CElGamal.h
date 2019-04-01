/*
Date: 22 Mar 2019 22:39:14.952269292
File: CElGamal.h

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

Purpose: ElGamal Encryption of messages.

Version control
09 Mar 2006 Duncan Camilleri           Initial development
10 Mar 2007 Duncan Camilleri           Completed encryption and decryption
28 Oct 2010 Duncan Camilleri           Introduced 16 bit prime + dependencies
28 Oct 2010 Duncan Camilleri           Introduced elgamal char
22 Mar 2019 Duncan Camilleri           Initial development
*/

#ifndef __CELGAMAL_H_CBADADB1628C586E3B2FA989A5F4CE16__
#define __CELGAMAL_H_CBADADB1628C586E3B2FA989A5F4CE16__

typedef struct _elchar {
   uint16_t msg;
   uint16_t ret;
} elchar;

// an ElGamal buffer constitutes of a elchar sequence
typedef struct _elbuf {
   elchar* m_bytes;
   uint32_t m_charCount;
} elbuf;

template <class T>
class CElGamal {
public:
   CElGamal(uint16_t lPrime, uint16_t lGenerator, uint16_t lPrivate); 
   ~CElGamal();

   void SetKeyPrivate(uint16_t l);
   uint16_t GetKeyPublic();

   elchar encode(T value, uint16_t lGenerator);
   T decode(elchar* pc);

protected: 
   uint16_t m_lP; // prime between 1000 and 10000
   uint16_t m_lG;

   uint16_t m_lprivate; // private key
   uint16_t m_lpublic;  // public key

   uint16_t doModulo(uint16_t g, uint16_t l);

public:
   static void printGenerators(uint16_t lPrime);
};

#endif   // __CELGAMAL_H_CBADADB1628C586E3B2FA989A5F4CE16__
