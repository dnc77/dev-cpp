/*
Date: 22 Mar 2019 22:39:14.835297030
File: becode.h

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

Purpose: A big endian data encoder/decoder.

Version control
13 Feb 2019 Duncan Camilleri           Initial development
19 Feb 2019 Duncan Camilleri           Removed byte* swap() function
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __BECODE_H_CF8D7A167175FE6F73F454BA473DB959__
#define __BECODE_H_CF8D7A167175FE6F73F454BA473DB959__

// Check for missing includes.
#if not defined _GLIBCXX_CSTDINT
#error "becode.h: missing include - cstdint"
#elif not defined _GLIBCXX_MUTEX
#error "cycbuf.h: missing include - mutex.h"       // std::once
#endif

// c++17 not installed. remove if compiling with a full c++17 support.
enum class byte : unsigned char {};

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

#endif   // __BECODE_H_CF8D7A167175FE6F73F454BA473DB959__
