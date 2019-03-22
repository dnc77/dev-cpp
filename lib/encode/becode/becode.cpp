/*
Date: 22 Mar 2019 22:39:22.370699225
File: becode.cpp

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

// Includes
#include <math.h>
#include <memory.h>
#include <sys/time.h>
#include <mutex>
#include <cstdint>
#include <becode.h>

std::once_flag becode::mFlagOnce;
bool becode::mIsBigEndian = false;

//
// CONSTRUCTOR/DESCTRUCTOR
//
becode::becode()
{
   // Detect endianness from the start.
   std::call_once(mFlagOnce, &isBigEndian);
}

becode::~becode()
{
}

//
// BYTE SWAPPING
//

void becode::swap(double& d)
{
   if (!mIsBigEndian) swap64((int64_t&)d);
}

void becode::swap(float& f)
{
   if (!mIsBigEndian) swap32((int32_t&)f);
}

void becode::swap(int64_t& n)
{
   if (!mIsBigEndian) swap64(n);
}

void becode::swap(int32_t& n)
{
   if (!mIsBigEndian) swap32(n);
}

void becode::swap(int16_t& n)
{
   if (!mIsBigEndian) swap16(n);
}

inline void becode::swap64(int64_t& bytes)
{
   bytes = (
      (0xFF00000000000000 & ((bytes) << 56)) |
      (0x00FF000000000000 & ((bytes) << 40)) |
      (0x0000FF0000000000 & ((bytes) << 24)) |
      (0x000000FF00000000 & ((bytes) << 8)) |
      (0x00000000FF000000 & ((bytes) >> 8)) |
      (0x0000000000FF0000 & ((bytes) >> 24)) |
      (0x000000000000FF00 & ((bytes) >> 40)) |
      (0x00000000000000FF & ((bytes) >> 56))
   );
}

inline void becode::swap32(int32_t& bytes)
{
   bytes = (
      (0xFF000000 & (bytes << 24)) |
      (0x00FF0000 & (bytes << 8)) |
      (0x0000FF00 & (bytes >> 8)) |
      (0x000000FF & (bytes >> 24))
   );
}

inline void becode::swap16(int16_t& bytes)
{
   bytes = (
      (0xFF00 & (bytes << 8)) |
      (0x00FF & (bytes >> 8))
   );
}

//
// ENDIAN DETECTION
//

// Detect endianness of a system. This only differentiates between
// big endian and little endian.
bool becode::isBigEndian()
{
   // If big endian, 1 will be in the right most bit of the right most byte.
   // If little endian, 1 will be in the right most bit of the left most byte.
   // Casting to a char* the address of the int will take us to the left most
   // byte.
   // If the left most byte is 0 (checked by the char* contents), then 
   // system is a big endian as the 1 would not be on this byte.
   // If the left most byte is 1 (checked by the char* contents), then
   // system is a little endian.
   int32_t endianness = 0x00000001;
   char* pc = (char*)&endianness;

   mIsBigEndian = ((*pc) == 0);
   return mIsBigEndian;
}

//
// FLOAT REPRESENTATION
//

// To convert a floating point to binary IEE754 form, the following
// needs to be done:
// 1: Divide the number by 2 until < 2 is reached.
//    Number of divisions done is the exponent.
// 2: Store exponent as excess 127.
//    This is done by adding 127 to the exponent and storing it
//    in a normal unsigned binary sequence.
// 3: From the result in 1, subtract 1 if greater than one and
//    multiply it by 2, storing integral bit in to the next bit
//    in the mantissa.
// 4: Step 3 keeps looping until either the number of bits available
//    are exhausted or until a result of 1.0 is encountered.
// Testing:
// 3.55 -> 0 10000000 1100011 00110011 00110011
//
bool becode::ieee754singleEnc(float f, byte* out, size_t outSize)
{
   if (outSize < 4 || out == nullptr) return false;
   memset(out, 0, outSize);

   if (f == 0.0) return true;

   // Deal with the sign bit first.
   int32_t value = 0;
   if (f < 0.0) {
      f = -f;
      value = 1 << 31;
   }

   // Excess 127 exponent.
   float divBy2 = f;
   uint32_t exponent = 127;
   while (divBy2 >= 2.0) {
      divBy2 /= 2.0;
      exponent++;
   }
   value |= (exponent << 23);

   // Last answer will not be included in bit sequence so
   // eliminate it.
   if (divBy2 >= 1.0) divBy2 -= 1.0;
   divBy2 *= 2;

   // Get mantissa next.
   int8_t shift = 22;
   while (shift >= 0) {
      if (divBy2 >= 1.0) {
         // Assign bit.
         value |= (0x00000001 << shift);

         // If 1.0, done.
         if (1.0 == divBy2) break;
            
         divBy2 -= 1.0;
      }

      // Next bit.
      shift--;
      divBy2 *= 2;
   }

   // Shift the remaining 9 bytes and swap.
   value << 9;
   swap(value);

   // Converted to IEEE754 single precision.
   memcpy(out, &value, 4);
   return true;
}

// Converts a 32 bit buffer representing an IEEE 754 single precision float
// into a float.
// Process:
// 1. Get sign bit (first bit) 1 = neg, 0 = positive.
// 2. Convert excess 127 bit number to decimal (bits 2 to 9).
// 3. Read in fraction (bits 10 to 32).
// 4. Define a point before the fraction and a 1 before it.
// 5. Move the point exponent times (- exponent moves backwards).
// 6. From the point to the left, power increments by 1 from 0.
// 7. From the point to the right, power decrements by -1 from -1.
// 8. For each bit, calculate 2 to the power of the power for that bit.
// 9. Add all results together to get the float.
bool becode::ieee754singleDec(float& f, byte* in)
{
   // Is this a 0?
   if ((int32_t)(*in) == 0) {
      f = 0;
      return true;
   }

   // Detect sign bit first.
   bool sign = false;
   char* pc = reinterpret_cast<char*>(in);
   if ((pc[0] & 0b10000000) == 0b10000000) sign = true;

   // Establish the exponent.
   uint8_t exponent = (pc[0] & 0b01111111);
   exponent = exponent << 1;
   exponent |= (pc[1] >> 7);
   int8_t signedexp = exponent - 127;

   // Having found the exponent, overwrite the last exponent bit
   // with 1 for denormalization. The point lies behind this one
   // initially but will be moved by exponent (left or right).
   // The power of the bit at 0b10000000 in byte 1 becomes the
   // exponent and subsequently keeps getting deducted by 1.
   byte pc1 = (byte)pc[1];
   pc[1] |= 0b10000000;

   // Find the float.
   f = 0.0;
   uint8_t offset = 0;
   while (offset < 24) {
      // Move to next byte?
      uint8_t byteoffset = offset % 8;
      if (0 == byteoffset) pc++;

      // Find fraction.
      if (pc[0] & (0b10000000 >> byteoffset)) {
         f += pow(2, signedexp);
      }

      // Move on to the next exponent.
      signedexp--;
      offset++;
   }

   // Set sign.
   if (sign) f = -f;

   // Maintain previous buffer and return.
   in[1] = pc1;
   return true;
}

// To convert a floating point to binary IEE754 form, the following
// needs to be done:
// 1: Divide the number by 2 until < 2 is reached.
//    Number of divisions done is the exponent.
// 2: Store exponent as excess 1023.
//    This is done by adding 1023 to the exponent and storing it
//    in a normal unsigned binary sequence.
// 3: From the result in 1, subtract 1 if greater than one and
//    multiply it by 2, storing integral bit in to the next bit
//    in the mantissa.
// 4: Step 3 keeps looping until either the number of bits available
//    are exhausted or until a result of 1.0 is encountered.
//
bool becode::ieee754doubleEnc(double f, byte* out, size_t outSize)
{
   if (outSize < 8 || out == nullptr) return false;
   memset(out, 0, outSize);

   if (f == 0.0) return true;

   // Deal with the sign bit first.
   int64_t value = 0;
   if (f < 0.0) {
      f = -f;
      value = 1;
      value = value << 63;
   }

   // Excess 1023 exponent.
   double divBy2 = f;
   uint64_t exponent = 1023;
   while (divBy2 >= 2.0) {
      divBy2 /= 2.0;
      exponent++;
   }
   value |= (exponent << 52);

   // Last answer will not be included in bit sequence so
   // eliminate it.
   if (divBy2 >= 1.0) divBy2 -= 1.0;
   divBy2 *= 2;

   // Get mantissa next.
   int8_t shift = 51;
   int64_t onebit = 1;
   while (shift >= 0) {
      if (divBy2 >= 1.0) {
         // Assign bit.
         value |= ((int64_t)onebit << shift);

         // If 1.0, done.
         if (1.0 == divBy2) break;
            
         divBy2 -= 1.0;
      }

      // Next bit.
      shift--;
      divBy2 *= 2;
   }

   // Shift the remaining 12 bytes and swap.
   value << 12;
   swap(value);

   // Converted to IEEE754 double precision.
   memcpy(out, &value, 8);
   return true;
}

// Converts a 64 bit buffer representing an IEEE 754 double precision float
// into a double.
// Process:
// 1. Get sign bit (first bit) 1 = neg, 0 = positive.
// 2. Convert excess 1023 bit number to decimal (bits 2 to 11).
// 3. Read in fraction (bits 12 to 64).
// 4. Define a point before the fraction and a 1 before it.
// 5. Move the point exponent times (- exponent moves backwards).
// 6. From the point to the left, power increments by 1 from 0.
// 7. From the point to the right, power decrements by -1 from -1.
// 8. For each bit, calculate 2 to the power of the power for that bit.
// 9. Add all results together to get the float.
bool becode::ieee754doubleDec(double& f, byte* in)
{
   // Is this a 0?
   if ((int64_t)(*in) == 0) {
      f = 0;
      return true;
   }

   // Detect sign bit first.
   bool sign = false;
   char* pc = reinterpret_cast<char*>(in);
   if ((pc[0] & 0b10000000) == 0b10000000) sign = true;

   // Establish the exponent.
   uint16_t exponent = (pc[0] & 0b01111111);
   exponent = exponent << 4;
   exponent |= (pc[1] >> 4);
   int16_t signedexp = exponent - 1023;

   // Having found the exponent, overwrite the last exponent bit
   // with 1 for denormalization. The point lies behind this one
   // initially but will be moved by exponent (left or right).
   // The power of the bit at 0b10000000 in byte 1 becomes the
   // exponent and subsequently keeps getting deducted by 1.
   byte pc1 = (byte)pc[1];
   pc[1] |= 0b00010000;

   // Find the float.
   f = 0.0;
   uint8_t offset = 3;
   pc++;
   while (offset < 56) {
      // Move to next byte?
      uint8_t byteoffset = offset % 8;
      if (0 == byteoffset) pc++;

      // Find fraction.
      if (pc[0] & (0b10000000 >> byteoffset)) {
         f += pow(2, signedexp);
      }

      // Move on to the next exponent.
      signedexp--;
      offset++;
   }

   // Set sign.
   if (sign) f = -f;

   // Maintain previous buffer and return.
   in[1] = pc1;
   return true;
}
