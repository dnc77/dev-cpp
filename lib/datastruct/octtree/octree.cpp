#include <assert.h>
#include <stdio.h>
#include <memory.h>

#include <vector>
#include <cfloat>

#include "octree.h"

using namespace std;

Octree::Octree()
{
   memset(&mBounds, 0, sizeof(aabb));
}

Octree::Octree(aabb& bounds)
{
   memcpy(&mBounds, &bounds, sizeof(aabb));
}

Octree::~Octree()
{
   // Nothing to free.
}

// Add an item to the octree.
// We're adding a bounding cube to the tree which will enclose an object of id
// 'id'. This is how it will work:
// 1. Check if this octree is a leaf node or an intermediate node.
// 1a.   If a leaf node, then find out if more than one child cube enclose the
//          object bounds. If they do, then promote the leaf node to an
//          intermediate node, create leaf nodes and recurse add into the
//          appropriate leaf nodes otherwise, just add the item to this node
//          and leave it as a leaf node.
// 2. If an intermediate node, then find out to which child nodes this object
//    belongs to and add it to them.
void Octree::Add(aabb const& bounds, int id)
{
   // Create the item first.
   Item item;
   item.mId = id;
   item.mBounds = bounds;

   // Do not add the item if it is not at least partially enclosed 
   // within the bounds of this node.
   if (!isItemInBoundsPartial(mBounds, item)) {
      return;
   }

   // Am I a leaf node?
   if (isLeaf()) {
      // Yes this is a leaf - Add item to this node first.
      mItems.push_back(item);

      // Should this leaf node be promoted to an intermediate node, will this
      // item be contained in more than one child leaf node? If yes, then just
      // promote this node.
      int count = 0;
      aabb pChildBounds[8];
      getChildBounds(mBounds, pChildBounds);
      for (int n = 0; n < 8 && count < 2; ++n) {
         if (isItemInBoundsPartial(pChildBounds[n], item)) {
            count++;
         }
      }

      // Promote node to an intermediate one?
      if (count >= 2) {
         // Only promote the node of this item does not cover the entire area
         // of this leaf's space.
         if (!isCubeEnclosed(item.mBounds, mBounds)) {
            promote();
         }
      }
   } else if (mLeaves.size() == 8) {         // must always be 8.
      // No - this is intermediate.
      for (vector<Octree>::iterator it = mLeaves.begin(); 
         it != mLeaves.end(); ++it)
      {
         it->Add(bounds, id);
      }
   }
}

// Clear the octree.
void Octree::Reset()
{
   memset(&mBounds, 0, sizeof(aabb));
   mItems.clear();
   mLeaves.clear();
}

void Octree::printf()
{
   // Intermediates do not have items.
   if (mItems.size() == 0) {
      ::printf("(Inode - x: %f - %f, y: %f - %f, z: %f - %f)\n", 
         mBounds.min.x, mBounds.max.x,
         mBounds.min.y, mBounds.max.y,
         mBounds.min.z, mBounds.max.z);
      
      // Print leaves!
      for (vector<Octree>::iterator it = mLeaves.begin(); 
         it != mLeaves.end(); ++it)
      {
         it->printf();
      }
   } else {
      ::printf("(Lnode - x: %f - %f, y: %f - %f, z: %f - %f)\n", 
         mBounds.min.x, mBounds.max.x,
         mBounds.min.y, mBounds.max.y,
         mBounds.min.z, mBounds.max.z);

      // Print items!
      for (vector<Item>::iterator it = mItems.begin(); 
         it != mItems.end(); ++it)
      {
         ::printf("   (item %d - x: %f - %f, y: %f - %f, z: %f - %f)\n",
            it->mId, 
            it->mBounds.min.x, it->mBounds.max.x,
            it->mBounds.min.y, it->mBounds.max.y,
            it->mBounds.min.z, it->mBounds.max.z);
      }
   }
}

// Find up to 'maxResults' intersecting items and write them into
// 'outResults' array. Returns the actual number of results stored.
int Octree::Query(Point const& point, int* outResults, int maxResults) const
{
   // If the leaf node containing this point is identified, then the items
   // within that node are searched such that any ones which enclose or touch
   // the input point will be returned.
   if (!isLeaf()) {
      // Go through leaves and query...
      for (vector<Octree>::const_iterator it = mLeaves.begin(); 
         it != mLeaves.end(); ++it)
      {
         int found = it->Query(point, outResults, maxResults);

         // A point is a point - it can be found only in one spot. Once found,
         // the process can stop.
         if (found > 0) return found;
      }

      // Not found in the leaf nodes of this intermediate.
      return 0;
   }

   // This is a leaf node. This node may enclose this point.
   if (!isPointInBounds(point, mBounds)) return 0;

   // Point is within this leaf node. Validate parameters.
   if (!outResults || maxResults <= 0) return 0;

   // Go through all the items within this node and up until maxResults report
   // each only if it touches or encloses this point.
   int results = 0;
   for (vector<Item>::const_iterator it = mItems.begin(); 
      it != mItems.end() && results < maxResults; ++it)
   {
      if (isPointInBounds(point, it->mBounds)) {
         outResults[results] = it->mId;
         results++;
      }
   }

   // Done.
   return results;
}

// am I a leaf?
bool Octree::isLeaf() const
{
   return mLeaves.size() == 0;
}

// If this node is a leaf node, then it will be promoted to an intermediate node
// otherwise nothing happens.
void Octree::promote()
{
   if (mLeaves.size() > 0) return;           // not a leaf

   // To promote, 8 child nodes need to be created to represent 8 equal cubes
   // within the bounding cube of this tree.
   aabb pChildBounds[8];
   getChildBounds(mBounds, pChildBounds);
   for (int n = 0; n < 8; ++n) {
      mLeaves.push_back(Octree(pChildBounds[n]));
   }

   // Add each item in this node to the child nodes.
   for (vector<Item>::iterator it = mItems.begin(); it != mItems.end(); ++it) {
      for (vector<Octree>::iterator leaf = mLeaves.begin();
         leaf != mLeaves.end(); ++leaf)
      {
         leaf->Add(it->mBounds, it->mId);
      }
   }

   // Once done, delete all items.
   mItems.clear();
}

// Splits bounds into 8 halves (a cube split across 3 axis from the centre point
// of each axis). Will get the enclosing cube for the specified point within the
// bounds. Will return nodeIdxOutOfBounds when the point is invalid.
// The orientation of the axis is as follows:
// -x to +x: left to right.
// -y to +y: top to bottom.
// -z to +z: far to near
PointIdx Octree::findPos(aabb const& bounds, Point const& point)
{
   float halves[3];
   halves[0] = bounds.min.x + ((bounds.max.x - bounds.min.x) / 2);
   halves[1] = bounds.min.y + ((bounds.max.y - bounds.min.y) / 2);
   halves[2] = bounds.min.z + ((bounds.max.z - bounds.min.z) / 2);

   // Validate the point to be within bounds.
   if (point.x < bounds.min.x)   return nodeIdxOutOfBounds;
   if (point.x >= bounds.max.x)  return nodeIdxOutOfBounds;
   if (point.y < bounds.min.y)   return nodeIdxOutOfBounds;
   if (point.y >= bounds.max.y)  return nodeIdxOutOfBounds;
   if (point.z < bounds.min.z)   return nodeIdxOutOfBounds;
   if (point.z >= bounds.max.z)  return nodeIdxOutOfBounds;

   // Determine position of point within the bounds. This will be used to
   // determine which node should be used for this point.
   if (point.x >= halves[0]) {
      // Right hand side.
      if (point.y >= halves[1]) {
         // Bottom half.
         if (point.z >= halves[2]) {
            // Far end.
            return nodeIdxBtmRightNear;
         } else if (point.z < halves[2]) {
            // Closer.
            return nodeIdxBtmRightFar;
         }
      } else if (point.y < halves[1]) {
         // top half.
         if (point.z >= halves[2]) {
            // Far end.
            return nodeIdxTopRightNear;
         } else if (point.z < halves[2]) {
            // Closer.
            return nodeIdxTopRightFar;
         }
      }
   } else if (point.x < halves[0]) {
      // Left hand side.
      if (point.y >= halves[1]) {
         // Bottom half.
         if (point.z >= halves[2]) {
            // Far end.
            return nodeIdxBtmLeftNear;
         } else if (point.z < halves[2]) {
            // Closer.
            return nodeIdxBtmLeftFar;
         }
      } else if (point.y < halves[1]) {
         // top half.
         if (point.z >= halves[2]) {
            // Far end.
            return nodeIdxTopLeftNear;
         } else if (point.z < halves[2]) {
            // Closer.
            return nodeIdxTopLeftFar;
         }
      }   
   }

   return nodeIdxOutOfBounds;
}

// Fills childBounds with the 8 child bounds of bound. I.e., one large cube will
// be split into 8 equally sized child cubes. childbounds MUST be an array of 8
// aabb structs.
// The orientation of the axis is as follows:
// -x to +x: left to right.
// -y to +y: top to bottom.
// -z to +z: far to near
void Octree::getChildBounds(aabb const& bounds, aabb* childbounds)
{
   float halves[3];
   halves[0] = bounds.min.x + ((bounds.max.x - bounds.min.x) / 2.0);
   halves[1] = bounds.min.y + ((bounds.max.y - bounds.min.y) / 2.0);
   halves[2] = bounds.min.z + ((bounds.max.z - bounds.min.z) / 2.0);

   // Top left near.
   // smaller x plane (left)
   // smaller y plane (top)
   // larger z plane (near)
   childbounds[nodeIdxTopLeftNear].min.x = bounds.min.x;
   childbounds[nodeIdxTopLeftNear].max.x = halves[0] - FLT_MIN;
   childbounds[nodeIdxTopLeftNear].min.y = bounds.min.y;
   childbounds[nodeIdxTopLeftNear].max.y = halves[1] - FLT_MIN;
   childbounds[nodeIdxTopLeftNear].min.z = halves[2];
   childbounds[nodeIdxTopLeftNear].max.z = bounds.max.z - FLT_MIN;

   // Top right near.
   // larger x plane (right)
   // smaller y plane (top)
   // larger z plane (near)
   childbounds[nodeIdxTopRightNear].min.x = halves[0];
   childbounds[nodeIdxTopRightNear].max.x = bounds.max.x - FLT_MIN;
   childbounds[nodeIdxTopRightNear].min.y = bounds.min.y;
   childbounds[nodeIdxTopRightNear].max.y = halves[1] - FLT_MIN;
   childbounds[nodeIdxTopRightNear].min.z = halves[2];
   childbounds[nodeIdxTopRightNear].max.z = bounds.max.z - FLT_MIN;

   // Bottom left near.
   // smaller x plane (left)
   // larger y plane (bottom)
   // larger z plane (near)
   childbounds[nodeIdxBtmLeftNear].min.x = bounds.min.x;
   childbounds[nodeIdxBtmLeftNear].max.x = halves[0] - FLT_MIN;
   childbounds[nodeIdxBtmLeftNear].min.y = halves[1];
   childbounds[nodeIdxBtmLeftNear].max.y = bounds.max.y - FLT_MIN;
   childbounds[nodeIdxBtmLeftNear].min.z = halves[2];
   childbounds[nodeIdxBtmLeftNear].max.z = bounds.max.z - FLT_MIN;

   // Bottom right near.
   // larger x plane (right)
   // larger y plane (bottom)
   // larger z plane (near)
   childbounds[nodeIdxBtmRightNear].min.x = halves[0];
   childbounds[nodeIdxBtmRightNear].max.x = bounds.max.x - FLT_MIN;
   childbounds[nodeIdxBtmRightNear].min.y = halves[1];
   childbounds[nodeIdxBtmRightNear].max.y = bounds.max.y - FLT_MIN;
   childbounds[nodeIdxBtmRightNear].min.z = halves[2];
   childbounds[nodeIdxBtmRightNear].max.z = bounds.max.z - FLT_MIN;

   // Top left far.
   // smaller x plane (left)
   // smaller y plane (top)
   // smaller z plane (far)
   childbounds[nodeIdxTopLeftFar].min.x = bounds.min.x;
   childbounds[nodeIdxTopLeftFar].max.x = halves[0] - FLT_MIN;
   childbounds[nodeIdxTopLeftFar].min.y = bounds.min.y;
   childbounds[nodeIdxTopLeftFar].max.y = halves[1] - FLT_MIN;
   childbounds[nodeIdxTopLeftFar].min.z = bounds.min.z;
   childbounds[nodeIdxTopLeftFar].max.z = halves[2] - FLT_MIN;

   // Top right far.
   // larger x plane (right)
   // smaller y plane (top)
   // smaller z plane (far)
   childbounds[nodeIdxTopRightFar].min.x = halves[0];
   childbounds[nodeIdxTopRightFar].max.x = bounds.max.x - FLT_MIN;
   childbounds[nodeIdxTopRightFar].min.y = bounds.min.y;
   childbounds[nodeIdxTopRightFar].max.y = halves[1] - FLT_MIN;
   childbounds[nodeIdxTopRightFar].min.z = bounds.min.z;
   childbounds[nodeIdxTopRightFar].max.z = halves[2] - FLT_MIN;

   // Bottom left far.
   // smaller x plane (left)
   // larger y plane (bottom)
   // smaller z plane (far)
   childbounds[nodeIdxBtmLeftFar].min.x = bounds.min.x;
   childbounds[nodeIdxBtmLeftFar].max.x = halves[0] - FLT_MIN;
   childbounds[nodeIdxBtmLeftFar].min.y = halves[1];
   childbounds[nodeIdxBtmLeftFar].max.y = bounds.max.y - FLT_MIN;
   childbounds[nodeIdxBtmLeftFar].min.z = bounds.min.z;
   childbounds[nodeIdxBtmLeftFar].max.z = halves[2] - FLT_MIN;

   // Bottom right far.
   // larger x plane (right)
   // larger y plane (bottom)
   // smaller z plane (far)
   childbounds[nodeIdxBtmRightFar].min.x = halves[0];
   childbounds[nodeIdxBtmRightFar].max.x = bounds.max.x - FLT_MIN;
   childbounds[nodeIdxBtmRightFar].min.y = halves[1];
   childbounds[nodeIdxBtmRightFar].max.y = bounds.max.y - FLT_MIN;
   childbounds[nodeIdxBtmRightFar].min.z = bounds.min.z;
   childbounds[nodeIdxBtmRightFar].max.z = halves[2] - FLT_MIN;
}

// Returns true if the point 'ppt' falls within the bounds 'pAb'.
bool Octree::isPointInBounds(Point const& pt, aabb const& ab)
{
   if (pt.x < ab.min.x)    return false;
   if (pt.y < ab.min.y)    return false;
   if (pt.z < ab.min.z)    return false;
   if (pt.x > ab.max.x)    return false;
   if (pt.y > ab.max.y)    return false;
   if (pt.z > ab.max.z)    return false;

   // In very simple terms the point is within bounds.
   return true;
}

// Determines if the searchItem exists within searchArea in full or in part.
bool Octree::isItemInBoundsPartial(aabb const& searchArea, Item const& item)
{
   // Since this is a partial binding check, the item does not need to be wholly
   // within the search area. If any point of the cube can be found within the
   // search area, we got a hit!

   // Check to see if any of the corners of the item's cube is within the search
   // area. If at least one corner of the item's cube is within the search area,
   // then we can say that the search area, to a certain extent owns this object
   // id.
   const Point* const pMin = &item.mBounds.min;
   const Point* const pMax = &item.mBounds.max;

   // There may yet be a better way to do this...
   // Avoid loop?
   Point pt;
   for (int x = pMin->x;  x <= pMax->x; x += pMax->x - pMin->x) {
      pt.x = x;
      for (int y = pMin->y;  y <= pMax->y; y += pMax->y - pMin->y) {
         pt.y = y;
         for (int z = pMin->z;  z <= pMax->z; z += pMax->z - pMin->z) {
            pt.z = z;
            if (isPointInBounds(pt, searchArea)) {
               return true;
            }  
         }
      }
   }

   // Ok so none of the corners of the item's cube are within the search area.
   // This means that this item is:
   // a) either completely out of the search area.
   // b) or enclosing totally the search area
   //       (but obviously can't be fully enclosed within the search area).
   if (isCubeEnclosed(item.mBounds, searchArea))
      return true;

   // Nowhere to be seen!
   return false;
}

// Returns true if ptBig wholly encloses ptSmall.
bool Octree::isCubeEnclosed(aabb const& big, aabb const& small)
{
   // small located before big.
   if (small.min.x < big.min.x) return false;
   if (small.min.y < big.min.y) return false;
   if (small.min.z < big.min.z) return false;

   // small located after end of big.
   if (small.min.x > big.max.x) return false;
   if (small.min.y > big.max.y) return false;
   if (small.min.z > big.max.z) return false;

   // small cube starts inside big cube but overlaps edges :(.
   // Small cube goes home...
   if (small.max.x > big.max.x) return false;
   if (small.max.y > big.max.y) return false;
   if (small.max.z > big.max.z) return false;

   // Otherwise, small cube stays here - small cube is in big cube.
   return true;
}


