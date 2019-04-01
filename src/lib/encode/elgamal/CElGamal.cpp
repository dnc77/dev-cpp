/*
Date: 22 Mar 2019 22:39:22.601879524
File: CElGamal.cpp

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
22 Mar 2019 Duncan Camilleri           Added copyright notice
28 Mar 2019 Duncan Camilleri           Location of CElGamal.h changed
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <encode/CElGamal.h>

// Because elgamal is limited to prime numbers 1000 to 10000, it will only be
// limited to 16 bit integers. 
template class CElGamal<int8_t>;
template class CElGamal<uint8_t>;
template class CElGamal<int16_t>;
template class CElGamal<uint16_t>;

// Exponent calculation.
uint32_t exp(uint32_t x, uint32_t n) {
   uint32_t y = 1;
   uint32_t u = x;

   do {
      if (n % 2 == 1) {
         y = y * u;
      }

      n = n / 2;
      u = u * u;
   } while (n != 0);

   return y;
}



template <class T>
CElGamal<T>::CElGamal(uint16_t lPrime, uint16_t lGenerator, uint16_t lPrivate) 
: m_lP(lPrime), m_lG(lGenerator)
{
  assert(m_lG <= (m_lP - 2));
  SetKeyPrivate(lPrivate);
}

template <class T>
CElGamal<T>::~CElGamal() {
}

template <class T>
void CElGamal<T>::SetKeyPrivate(uint16_t l) {
  m_lprivate = l;
  m_lpublic = doModulo(m_lG, m_lprivate);
}

template <class T>
uint16_t CElGamal<T>::GetKeyPublic() {
  return m_lpublic;
}

template <class T>
elchar CElGamal<T>::encode(T value, uint16_t lGenerator) {
   assert(value < m_lP);

  // Initialize random number generation.
  srand(time(0));
  uint16_t l = value;

  // 1. generate random number k (1 <= k <= m_lP - 2)
  // 2. encode the random number to r (first part of crypted result)
  elchar el;
  uint16_t k = (uint16_t)(rand() % (m_lP - 2)) + 1;
  el.ret = doModulo(m_lG, k);

  // 3. calculate second part of crypted result (will be stored in plMessage)
  uint16_t x = doModulo(m_lpublic, k);
  el.msg = (l * x) % m_lP;
  return el;
}

template <class T>
T CElGamal<T>::decode(elchar* pc) {
  uint16_t iMultiplier = doModulo(pc->ret, m_lprivate);
  
  for (uint16_t nRet = 1; nRet < m_lP; ++nRet) {
    uint16_t modulo = (iMultiplier * nRet) % m_lP;
    if (modulo == pc->msg) {
      return (T)nRet;
    }
  }

  return 0;
}

// (g ^ l) % p
// because l can only be limited to m_lP - 1 and 1000 <= m_lP <= 10000 
// 16 bits is enough.
template <class T>
uint16_t CElGamal<T>::doModulo(uint16_t g, uint16_t l) {
  uint16_t ly = 1;
  uint16_t lu = g % m_lP;
  
  do {
    if (l % 2 == 1)    { ly = (ly * lu) % m_lP; }
    l /= 2;
    lu = (lu * lu) % m_lP;
  } while (l != 0);

  return ly;
}

// finds generator numbers and their private keys for prime number lPrime.
// The generator and prime number are both used together to correctly be 
// able to encrypt and decrypt a character successfully. It is assumed that
// lPrime is actually a prime number. If lPrime is not a prime number,
// decryption will not work. 
// generators are output to stdout for speed purposes.
template <class T>
void CElGamal<T>::printGenerators(uint16_t lPrime) {
  printf("ElGamal generator numbers and private keys for prime number %u\n",
     lPrime);

   // loop thru to prime-1 with exponents and generators. seek generators
  for (uint16_t lCnt = 1; lCnt < lPrime - 1; ++lCnt) {
    for (uint16_t lExp = 2; lExp < lPrime - 1; ++lExp) {
      uint32_t lResult = exp(lCnt, lExp);
      if (lResult > lPrime) {
        break;
      }
      
      if (lResult == (lPrime - 1)) {
        printf("Found Generator G[%u] with exponent %u: %u^%u=%u\n",
                  lCnt, lExp, lCnt, lExp, lResult);
      }
    }
  }

  printf ("\n");
}
