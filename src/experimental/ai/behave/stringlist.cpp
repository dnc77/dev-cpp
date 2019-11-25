/*
Date: 10 Oct 2019 09:09:41.853107249
File: stringlist.cpp

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

Copyright (C) 2000-2025 Duncan Camilleri, All rights reserved.
End of Copyright Notice

Purpose: Provides one buffer of memory that can be expanded to store multiple
         strings. The purpose of this is for strings to be allocated in one
         whole space rather than scattered around across different memory
         locations; possibly causing memory fragmentation.
         windowSize determines by how much the buffer is incremented every
         time a new allocation in memory needs to be done.

Version control
10 Oct 2019 Duncan Camilleri           Initial development
12 Oct 2019 Duncan Camilleri           memset() only once
13 Oct 2019 Duncan Camilleri           addString() failure return code fixed
13 Oct 2019 Duncan Camilleri           Added more comments to addString()
13 Oct 2019 Duncan Camilleri           Added joinWithLast()
16 Oct 2019 Duncan Camilleri           joinWithLast() checks for empty buffer
17 Oct 2019 Duncan Camilleri           Introduced copy and move operators
19 Oct 2019 Duncan Camilleri           Added buffer search
*/

#include <malloc.h>
#include <memory.h>
#include <cstdint>
#include <string.h>
#include "stringlist.h"

template class StringList<slsiz::tiny>;
template class StringList<slsiz::small>;
template class StringList<slsiz::medium>;
template class StringList<slsiz::large>;
template class StringList<slsiz::huge>;

template <slsiz windowSize>
StringList<windowSize>::StringList()
: mSpace(0), mUsed(0), mpBuffer(nullptr)
{
}

template <slsiz windowSize>
StringList<windowSize>::StringList(const StringList<windowSize>& sl)
{
   operator=(sl);
}

template <slsiz windowSize>
StringList<windowSize>::StringList(StringList<windowSize>&& sl)
{
   operator=(sl);
}

template <slsiz windowSize>
StringList<windowSize>::~StringList()
{
   reset();
}

//
// ASSIGNMENT OPERATORS
//

// When making a copy, do a deep copy of the memory buffer as well.
template <slsiz windowSize>
StringList<windowSize>& StringList<windowSize>::operator=(
   const StringList<windowSize>& sl)
{
   if (&sl == this)
      return *this;

   // Allocate new buffer.
   mpBuffer = (char*)malloc(sl.mSpace);
   if (!mpBuffer) {
      mSpace = 0;
      mUsed = 0;
   }

   // Copy source buffer.
   memcpy(mpBuffer, sl.mpBuffer, sl.mSpace);
   mSpace = sl.mSpace;
   mUsed = sl.mUsed;

   return *this;
}

// Move operator. When moving (non constant stringlist source),
// make sure source is cleared.
template <slsiz windowSize>
StringList<windowSize>& StringList<windowSize>::operator=(
   StringList<windowSize>&& sl)
{
   if (&sl == this)
      return *this;

   // Copy members.
   mpBuffer = sl.mpBuffer;
   mSpace = sl.mSpace;
   mUsed = sl.mUsed;

   // Clear source.
   sl.mpBuffer = nullptr;
   sl.mSpace = 0;
   sl.mUsed = 0;

   return *this;
}

//
// STRING MAINTENANCE
//

template <slsiz windowSize>
const char* StringList<windowSize>::operator[](sloffset off) const
{
   return mpBuffer + off;
}

// Purpose: Adds another string to the string list. If not enough space,
//          reallocates the buffer.
// Params:  str: string to add
//          length: length (excluding \0) of string to add
// Returns: sloffset of new string.
template <slsiz windowSize>
sloffset StringList<windowSize>::addString(
   const char* const str, size_t length)
{
   if (!str || length < 1) return sloffsetbad;

   // Check if new buffer needs to be allocated.
   uint64_t used = mUsed;
   uint64_t newUsed = mUsed + length + 1;
   uint64_t newSpace = mSpace;
   while (newUsed >= newSpace) {
      // TODO: Check if new size will use up the whole space!
      //       Convert to uint64
      newSpace += (const unsigned int)windowSize;
   }

   // Realloc buffer if needed.
   if (newSpace > mSpace) {
      char* pNewBuf = (char*)realloc((void*)mpBuffer, newSpace);
      if (!pNewBuf) {
         return sloffsetbad;
      } else {
         mSpace = newSpace;
         mpBuffer = pNewBuf;

         // Clean buffer.
         void* pClean = (void*)((char*)mpBuffer + used);
         size_t toClean = mSpace - used;
         memset((void*)pClean, 0, toClean);
      }
   }

   // Copy string data from str to current buffer.
   void* pTarget = (void*)(mpBuffer + mUsed);
   memcpy(pTarget, str, length);
   mUsed = newUsed;

   // Return pointer to newly allocated buffer.
   return used;
}

// Purpose: Joins string to the last string in the string list. If not enough
//          space, reallocates the buffer.
//          Note: For efficiency reasons, joinWithLast was introduced separately
//          to addString. It therefore depends on addString's functionality but
//          not without modifying internal status variables.
// Params:  str: string to add
//          length: length (excluding \0) of string to add
// Returns: sloffset of location of the join.
//          Note: The return code does NOT return the offset of the whole string
//          that is joined. Rather, the return code returned is the position of 
//          the new string that was joined. For efficiency reasons, it is better
//          to not have to locate the beginning of the whole string. The caller
//          should be responsible to know what the offset of the first string of
//          the whole join sequence was.
template <slsiz windowSize>
sloffset StringList<windowSize>::joinWithLast(
   const char* const str, size_t length)
{
   if (!str || length < 1) return sloffsetbad;

   // Discard last terminator by treating it as free space.
   if (mUsed > 0) mUsed--;

   // Add the new string and if this fails, reset the data and resize the buffer
   // to include a terminator.
   sloffset added = addString(str, length);
   if (sloffsetbad != added)
      return added;

   // Failed. Fix buffer.
   memset(mpBuffer + ++mUsed, 0, length);
   return sloffsetbad;
}

// Empty the buffer.
template <slsiz windowSize>
void StringList<windowSize>::reset()
{
   mSpace = 0;
   mUsed = 0;
   if (nullptr != mpBuffer) {
      free((void*)mpBuffer);
      mpBuffer = nullptr;
   }
}

//
// Searching
//

// Locate string in buffer and return position.
// Returns sloffsetbad if not found.
template <slsiz windowSize>
sloffset StringList<windowSize>::search(const char* const string)
{
   if (nullptr == mpBuffer || mUsed == 0) return sloffsetbad;

   // Validate buffer big enough.
   uint64_t size = strlen(string) + 1;
   if (mUsed < size) return sloffsetbad;

   // Locate buffer.
   const char* const found =
      (const char* const)memmem(mpBuffer, mUsed, string, size);
   if (!found) return sloffsetbad;

   // Calculate and return offset.
   return (sloffset)(found -  mpBuffer);
}
