#include <assert.h>
#include <stdio.h>
#include <memory.h>

#include <vector>
#include <cfloat>

#include "octree.h"

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


