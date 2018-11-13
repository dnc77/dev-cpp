// Author: Copyright (C) 2014 Duncan Camilleri (dnc77@hotmail.com) 

#include <assert.h>
#include <stdio.h>
#include <memory.h>

#include <vector>
#include <cfloat>

#include "octree.h"

Octree::Octree()
   : mId(0)
{
   // Initialise.
   memset(&mPoint, 0, sizeof(mPoint));
   mSet = false;                              
   mChildren = 0;
}

Octree::~Octree()
{
   if (mChildren)
      delete[] mChildren;
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
   if (mLeaves.size() == 0) {
      // Yes this is a leaf - Add item to this node first.
      mItems.push_back(item);

      // If this leaf node is promoted to an intermediate node, will this
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
      if (count > 2) {
         promote();
      }
   } else if (mLeaves.size() == 8) {         // must always be 8.
      // No - this is intermediate.
      for (int n = 0; n < 8; ++n) {
         mLeaves[n].Add(bounds, id);
      }
   }
}

// Clear the octree.
void Octree::Reset()
{

}

// Find up to 'maxResults' intersecting items and write them into
// 'outResults' array. Returns the actual number of results stored.
int Octree::Query(Point const& point, int* outResults, int maxResults) const
{

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
   halves[0] = bounds.min.x + ((bounds.max.x - bounds.min.x) / 2)
   halves[1] = bounds.min.y + ((bounds.max.y - bounds.min.y) / 2)
   halves[2] = bounds.min.z + ((bounds.max.z - bounds.min.z) / 2)

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
   halves[0] = bounds.min.x + ((bounds.max.x - bounds.min.x) / 2)
   halves[1] = bounds.min.y + ((bounds.max.y - bounds.min.y) / 2)
   halves[2] = bounds.min.z + ((bounds.max.z - bounds.min.z) / 2)

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
bool Octree::isPointInBounds(Point* ppt, aabb* pAb)
{
   if (ppt->x < pAb->min.x)   return false;
   if (ppt->y < pAb->min.y)   return false;
   if (ppt->z < pAb->min.z)   return false;
   if (ppt->x >= pAb->max.x)  return false;
   if (ppt->y >= pAb->max.y)  return false;
   if (ppt->z >= pAb->max.z)  return false;

   // In very simple terms the point is within bounds.
   return true;
}

// Determines if the searchItem exists within searchArea in full or in part.
bool Octree::isItemInBoundsPartial(aabb const& searchArea, Item const& item)
{
   return true;
}
