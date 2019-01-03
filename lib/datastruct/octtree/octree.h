#ifndef __OCTREE_H__
#define __OCTREE_H__

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

#endif   // __OCTREE_H__


