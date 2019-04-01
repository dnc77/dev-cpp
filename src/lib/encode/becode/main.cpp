/*
Date: 22 Mar 2019 22:39:22.486323646
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

Purpose: Big endian encoding class testing - floating point number test.

Version control
22 Mar 2019 Duncan Camilleri           Added copyright notice
31 Mar 2019 Duncan Camilleri           helpers.h needed for byte buffer

*/

#include <cstdint>
#include <mutex>
#include <memory.h>
#include <sys/time.h>                  // helpers.h
#include <helpers.h>
#include <encode/becode.h>

void ieee753ToBufTest()
{
   byte flb[4];
   becode bc;

   auto sng = [&](float fl) {
      float flSwapped = fl;
      float flWriteBack = 0;
      bc.swap(flSwapped);
      bc.ieee754singleEnc(fl, flb, 4);
      bc.ieee754singleDec(flWriteBack, flb);
      printf("ieee753 %f to buffer 0x%04x = '%s' to float: %f (%s)\n",
         fl, flb,
         (memcmp(&flSwapped, flb, 4) == 0) ? "match" : "fail",
         flWriteBack,
         (flWriteBack == fl) ? "match" : "fail"
      );
   };

   byte dbb[8];
   auto dbl = [&](double fl) {
      double flSwapped = fl;
      double flWriteBack = 0;
      bc.swap(flSwapped);
      bc.ieee754doubleEnc(fl, dbb, 8);
      bc.ieee754doubleDec(flWriteBack, dbb);
      printf("ieee753 %f to buffer 0x%08x = '%s' to double: %f (%s)\n",
         fl, dbb,
         (memcmp(&flSwapped, dbb, 8) == 0) ? "match" : "fail",
         flWriteBack,
         (flWriteBack == fl) ? "match" : "fail"
      );
   };

   sng(-3.55);
   sng(3.55);
   sng(83.25);
   sng(31.8);
   sng(1.0);
   sng(-1.0);
   sng(324752.1);
   sng(0.0);
   sng((float)0xffffffff);

   dbl(-3.55);
   dbl(3.55);
   dbl(83.25);
   dbl(-18211.58);
   dbl(1.0);
   dbl(-1.0);
   dbl(37567752.124122171);
   dbl(0.0);
   dbl((double)0xffffffffffffffff);
}

int main(int argc, char** argv)
{
   ieee753ToBufTest();
   return 0;
}

