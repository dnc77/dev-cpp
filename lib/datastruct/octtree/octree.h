#ifndef __OCTREE_H__
#define __OCTREE_H__

// Author: Copyright (C) 2014 Duncan Camilleri (dnc77@hotmail.com) 

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

// A flag to determine in which nodes an object falls under.
typedef enum _presence {
   nodeNone = 0x0000,
   nodeTopLeftNear = 0x0001,
   nodeTopRightNear = 0x0002,
   nodeBtmLeftNear = 0x0004,
   nodeBtmRightNear = 0x0008,
   nodeTopLeftFar = 0x0010,
   nodeTopRightFar = 0x0020,
   nodeBtmLeftFar = 0x0040,
   nodeBtmRightFar = 0x0080
} WhichNodes;

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
public:
   Octree();
   virtual ~Octree();

   // Add an item to the octree.
   void Add(aabb const& bounds, int id);

   // Clear the octree.
   void Reset();

   // Find up to 'maxResults' intersecting items and write them into
   // 'outResults' array. Returns the actual number of results stored.
   int Query(Point const& point, int* outResults, int maxResults) const;

private:
   aabb mBounds;                    // as a leaf node this is the bounding rect
   std::vector<Octree> mLeaves;     // as an intermediate node this is filled
   std::vector<Item> mItems;        // as an intermediate node this is filled

   static PointIdx findPos(aabb const& bounds, Point const& point);
   static void getChildBounds(aabb const& bounds, aabb* childbounds);  
   static bool isPointInBounds(Point* ppt, aabb* pAb);
   static bool isItemInBoundsPartial(aabb const& searchArea, Item const& item);
};

#endif   // __OCTREE_H__

