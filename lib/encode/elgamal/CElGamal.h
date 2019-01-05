// File: CElGamal.cpp
// Purpose: ElGamal Encryption of messages
// Headers: hlpTypes.h
// Changes
// 09 Mar 2006: Duncan Camilleri - Initial development
// 10 Mar 2007: Duncan Camilleri - Completed encryption and decryption
// 28 Oct 2010: Duncan Camilleri - Introduced 16 bit prime + dependencies
// 28 Oct 2010: Duncan Camilleri - Introduced elgamal char. 

#ifndef __CELGAMAL_H__
#define __CELGAMAL_H__

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


#endif    // __CELGAMAL_H__

