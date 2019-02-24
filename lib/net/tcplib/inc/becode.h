// Date:    13th February 2019
// Purpose: A big endian data encoder/decoder.
//
// Version control
// 13 Feb 2019 Duncan Camilleri           Initial development
// 19 Feb 2019 Duncan Camilleri           Removed byte* swap() function
//

#ifndef __BECODE_H__
#define __BECODE_H__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "becode.h: missing include - cstdint"
#elif not defined _GLIBCXX_MUTEX
#error "cycbuf.h: missing include - mutex.h"       // std::once
#elif not defined __HELPERS_H__
#error "cycbuf.h: missing include - helpers.h"     // byte definition (no c++17)
#endif

class becode
{
public:
   becode();
   virtual ~becode();

   // Byte swapping.
   void swap(float& f);
   void swap(double& d);
   void swap(int64_t& n);
   void swap(int32_t& n);
   void swap(int16_t& n);

private:
   void swap64(int64_t& bytes);
   void swap32(int32_t& bytes);
   void swap16(int16_t& bytes);

   // Endian detection (run only once and is static)
   static std::once_flag mFlagOnce;            // detect only once
   static bool mIsBigEndian;
   static bool isBigEndian();

public:
   // Float representation.
   bool ieee754singleEnc(float f, byte* out, size_t outSize);
   bool ieee754singleDec(float& f, byte* in);
   bool ieee754doubleEnc(double f, byte* out, size_t outSize);
   bool ieee754doubleDec(double& f, byte* in);
};


#endif      // __BECODE_H__
