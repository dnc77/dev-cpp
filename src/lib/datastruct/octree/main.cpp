/*
Date: 22 Mar 2019 22:39:20.745460899
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

Purpose: Octree sampler.

Version control
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#include <assert.h>
#include <stdio.h>
#include <memory.h>

#include <vector>
#include <cfloat>

#include "datastruct/octree.h"

using namespace std;

int main(int argc, char** argv)
{
   // some very quick tests.
   aabb bounds;
   bounds.min.x = 0;
   bounds.min.y = 0;
   bounds.min.z = 0;
   bounds.max.x = 10;
   bounds.max.y = 10;
   bounds.max.z = 10;
   Octree o(bounds);

   Item bob;
   Item alice;
   Item jim;
   bob.mId = 1;

   bob.mBounds.min.x = 2;
   bob.mBounds.min.y = 2;
   bob.mBounds.min.z = 2;
   bob.mBounds.max.x = 4;
   bob.mBounds.max.y = 4;
   bob.mBounds.max.z = 4;

   alice.mId = 2;
   alice.mBounds.min.x = 3;
   alice.mBounds.min.y = 3;
   alice.mBounds.min.z = 3;
   alice.mBounds.max.x = 8;
   alice.mBounds.max.y = 8;
   alice.mBounds.max.z = 8;

   o.Add(bob.mBounds, bob.mId);
   o.Add(alice.mBounds, alice.mId);
   o.printf();

   // Intersects..
   Point pt;
   int results[3];
   int intersect;

   printf("Intersects at 3, 3, 3\n");
   pt.x = 3; pt.y = 3; pt.z = 3;
   intersect = o.Query(pt, results, 3);
   for (int n = 0; n < intersect; ++n)
      printf("\t%d\n", results[n]);

   printf("Intersects at 2, 2, 2\n");
   pt.x = 2; pt.y = 2; pt.z = 2;
   intersect = o.Query(pt, results, 3);
   for (int n = 0; n < intersect; ++n)
      printf("\t%d\n", results[n]);

   printf("Intersects at 8, 8, 8\n");
   pt.x = 8; pt.y = 8; pt.z = 8;
   intersect = o.Query(pt, results, 3);
   for (int n = 0; n < intersect; ++n)
      printf("\t%d\n", results[n]);

   printf("Intersects at 9, 9, 9\n");
   pt.x = 9; pt.y = 9; pt.z = 9;
   intersect = o.Query(pt, results, 3);
   for (int n = 0; n < intersect; ++n)
      printf("\t%d\n", results[n]);

   return 0;
}
