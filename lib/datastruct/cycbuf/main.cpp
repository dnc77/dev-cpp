/*
Date: 22 Mar 2019 22:39:20.977018449
File: main.cpp

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

Purpose: Sample cyclic buffer test.

Version control
22 Mar 2019 Duncan Camilleri           Added copyright notice
22 Mar 2019 Duncan Camilleri           Fixed bug with pushRead() old name call
*/

#include <string>
#include <memory.h>
#include "cycbuf.h"

using namespace std;

// Tests the cyclic buffer
void cyclicTest()
{
   int ret = 0;
   cycbuf<tiny> cyc;
   byte buf[5];

   auto rw = [&](byte* data, size_t size, bool write) {
      string out = cyc.toString();
      printf("%s", out.c_str());

      if (write) {
         ret = cyc.writecopy(data, size);
         printf(" <== write: '%s' => written: %d ==> ", data, ret);
      } else {
         ret = cyc.readcopy(data, size);
         printf(" ==> tryrd: %d bytes => read: %d bytes => '%s' ==> ",
            size, ret, data
         );
      }

      out = cyc.toString();
      printf("%s\n", out.c_str());
   };

   rw((byte*)"1234", 4, true);
   rw((byte*)"5678", 4, true);
   rw((byte*)"9abcd", 5, true);
   memset(buf, 0, 5); rw(buf, 4, false);
   memset(buf, 0, 5); rw(buf, 4, false);
   memset(buf, 0, 5); rw(buf, 4, false);

   rw((byte*)"1234", 4, true);      // empty ht at end
   memset(buf, 0, 5); rw(buf, 4, false);
   rw((byte*)"5678", 4, true);
   memset(buf, 0, 5); rw(buf, 4, false);
   rw((byte*)"9abcd", 5, true);
   rw((byte*)"efghi", 5, true);
   memset(buf, 0, 5); rw(buf, 4, false);   
   rw((byte*)"jklmnopqrstu", 12, true);
   rw((byte*)"v", 1, true);
   memset(buf, 0, 5); rw(buf, 3, false);
   rw((byte*)"wxyz", 4, true);
}

// Tests the cyclic buffer via direct access.
void cyclicdaTest()
{
   cycbuf<tiny> cyc;
   string out;

   auto rw = [&](byte* data, size_t size, bool write) {
      out = cyc.toString(); //cyc.show(false);
      printf("%s", out.c_str());

      if (write) {
         // First get the write buffer and size.
         size_t s = 0;
         byte* buf = cyc.getWriteTail(s);
         s = min(size, s);

         // Write memory to buffer.
         memcpy(buf, data, min(size, s));
         cyc.pushWriteTail(min(size, s));
         printf(" <== write: '%s' => written: %d ==> ", data, s);
      } else {
         // First get the read buffer and size.
         size_t s = 0;
         byte const* buf = cyc.getReadHead(s);
         printf(" ==> read: %d bytes => '%s' ==> ", s, buf);
         cyc.pushReadHead(s);
      }

      out = cyc.toString();
      printf("%s\n", out.c_str());
      // cyc.show(true);
   };

   rw((byte*)"1234", 4, true);
   rw((byte*)"5678", 4, true);
   rw((byte*)"9abcd", 5, true);
   rw(nullptr, 4, false);
   rw(nullptr, 4, false);
   rw(nullptr, 4, false);

   rw((byte*)"1234", 4, true);      // empty ht at end
   rw(nullptr, 4, false);
   rw((byte*)"5678", 4, true);
   rw(nullptr, 4, false);
   rw((byte*)"9abcd", 5, true);
   rw((byte*)"efghi", 5, true);
   rw(nullptr, 4, false);   
   rw((byte*)"jklmnopqrstu", 12, true);
   rw((byte*)"v", 1, true);
   rw(nullptr, 3, false);
   rw((byte*)"wxyz", 4, true);
}

int main(int argc, char** argv)
{
   printf("Copy cyclic buffer test\n");
   printf("-----------------------\n");
   cyclicTest();

   printf("\n");
   printf("Direct access cyclic buffer test\n");
   printf("--------------------------------\n");
   cyclicdaTest();
   return 0;
}
