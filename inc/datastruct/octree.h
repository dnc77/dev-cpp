/*
Date: 22 Mar 2019 22:39:13.329256968
File: octree.h

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

Purpose: A 3D octagonal tree for collision detection and more.

Version control
22 Mar 2019 Duncan Camilleri           Added copyright notice
*/

#ifndef __OCTREE_H_C320E78EB2172B4CB9E10507D5D96B4A__
#define __OCTREE_H_C320E78EB2172B4CB9E10507D5D96B4A__

//
// Some terminology:
//
// Octree:        a tree where each node has 8 child nodes. Typically used in
//                3D graphics to represent a cube which is split across the 3
//                axis from the centre point forming 8 'child' cubes.
// Intermediate:  a node that has child nodes. A big cube which has 8 smaller
//                equally sized child cubes.
// Leaf:          a node which is not split any further. It will be a bounding
//                cube containing a list of object references it encloses.
struct Point
{
   float x;
   float y;
   float z;
};

struct aabb
{
   Point min;
   Point max;
};

struct Item
{
   int mId;
   aabb mBounds;
};

// Const pointers to const structs.
typedef const Point* const CPointPtr;
typedef const aabb* const CAabbPtr;

// A sequence identifying the index of each node in a vector.
typedef enum _pointIdx {
   nodeIdxTopLeftNear = 0x0000,
   nodeIdxTopRightNear = 0x0001,
   nodeIdxBtmLeftNear = 0x0002,
   nodeIdxBtmRightNear = 0x0003,
   nodeIdxTopLeftFar = 0x0004,
   nodeIdxTopRightFar = 0x0005,
   nodeIdxBtmLeftFar = 0x0006,
   nodeIdxBtmRightFar = 0x0007,
   nodeIdxOutOfBounds = 0xffff
} PointIdx;

class Octree
{
private:
   Octree();

public:
   Octree(aabb& bounds);
   virtual ~Octree();

   // Add an item to the octree.
   void Add(aabb const& bounds, int id);

   // Clear the octree.
   void Reset();

   //
   // OCTREE FUNCTIONALITY
   //

   // Console me!
   void printf();

   // Find up to 'maxResults' intersecting items and write them into
   // 'outResults' array. Returns the actual number of results stored.
   int Query(Point const& point, int* outResults, int maxResults) const;

private:
   aabb mBounds;                    // as a leaf node this is the bounding rect
   std::vector<Octree> mLeaves;     // as an intermediate node this is filled
   std::vector<Item> mItems;        // as a leaf node this may be filled

   bool isLeaf() const;             // am I a leaf?
   void promote();                  // promotes a leaf node to intermediate

   //
   // STATIC HELPERS.
   //

   // Determines an index which identifies the child node of an intermediate
   // based on the position of the point within bounds.
   static PointIdx findPos(aabb const& bounds, Point const& point);

   // Assumes an array of 8 bounds which will be filled with the bounds of the
   // child cubes of bounds.
   static void getChildBounds(aabb const& bounds, aabb* childbounds);  

   // Returns true of pt lies within ab.
   static bool isPointInBounds(Point const& pt, aabb const& ab);

   // Returns true if item exists partially within the search area.
   static bool isItemInBoundsPartial(aabb const& searchArea, Item const& item);

   // Returns true if the big bounds wholly enclose the small bounds.
   static bool isCubeEnclosed(aabb const& big, aabb const& small);
};

#endif   // __OCTREE_H_C320E78EB2172B4CB9E10507D5D96B4A__
