/*
Date: 10 Oct 2019 09:09:35.086542914
File: stringlist.h

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
13 Oct 2019 Duncan Camilleri           Buffer should not be a const
19 Oct 2019 Duncan Camilleri           Added buffer search
07 Nov 2019 Duncan Camilleri           Added sloffsettop
*/

#ifndef __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__
#define __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__

#if not defined _GLIBCXX_CSTDINT
#error "stringlist.h: missing include - cstdint"
#endif

using sloffset = uint64_t;
constexpr uint64_t sloffsettop = 0;
constexpr uint64_t sloffsetbad = UINT64_MAX; // using last integer as bad offset
                                             // no point pointing to no data

// Different window sizes.
enum class slsiz : const unsigned int {
   tiny = 16,
   small = 64,
   medium = 512,
   large = 1024,
   huge = 4096
};

template <slsiz windowSize>
class StringList
{
public:
   StringList();
   StringList(const StringList<windowSize>& sl);
   StringList(StringList<windowSize>&& sl);
   virtual ~StringList();

   // Assignment operators.
   StringList<windowSize>& operator=(const StringList<windowSize>& sl);
   StringList<windowSize>& operator=(StringList<windowSize>&& sl);

   // String maintenance.
   const char* operator[](sloffset off) const;
   sloffset addString(const char* const str, size_t length);
   sloffset joinWithLast(const char* const str, size_t length);

   void reset();

   // Searching
   sloffset search(const char* const string);

private:
   uint64_t mSpace;
   uint64_t mUsed;
   char* mpBuffer;
};

#endif   // __STRINGLIST_H_3F91B722978DC8F00D1E31EF68DC35C8__


/*
void usingsl()
{
   char* pp = "djasdhjhsadhih";
   vector<sloffset> v;
   StringList<slsiz::medium> a;
   for (int c = 0; c < 10000; ++c) {
      v.push_back(a.addString(pp, strlen(pp)));
   }
}  

void usingStr()
{
   vector<string> v;
   for (int c = 0; c < 10000; ++c) {
      v.push_back("djasdhjhsadhih");
   }
}

   clock_t start = 0;
   clock_t stop = 0;
   double elapsed = 0.0;

   start = clock();
   usingsl();
   stop = clock();
   elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
   printf("usingsl elapsed in ms: %f\n", elapsed);

   start = clock();
   usingStr();
   stop = clock();
   elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC;
   printf("usingStr elapsed in ms: %f\n", elapsed);


*/
