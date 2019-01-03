octree

Summary:
This is a representation of a 3D space with 3D objects within it.
The model allows for the quick detection of one or more objects within a
particular space or point. This can be used for the detection of collision in a
game as well as for other purposes.

How it works:
An octree is a definition of a 3D space in the form of two 3D points: a min and
a max.

An octree can be either an intermediate or a leaf.

When an octree is intermediate, it is said to only have 8 equal child octrees
taking up the whole space of the intermediate. No objects (items) exist in
intermediate octrees.

When an octree is a leaf, it does not have child octrees. It may however have
one or more objects (represented as items) that take up some space within the
octree bounds.

When 3D objects are added to the parent octree, it is traversed until the
smallest containing octrees are found. When an intermediate octree is found, the
item will be added to all child octrees that enclose any part of the space
occupied by the 3D object. Since only leaf nodes can contain items, the process 
keeps iterating until all containing leaf nodes are found. When one leaf node
contains the 3D object, it may be promoted to an intermediate node if necessary
so that the object is stored within multiple leaf nodes of that newly promoted
intermediate node. 

How to use:
1: Create a parent octree of a specific size (using aabb structure to define
   the bounds).
2: Create a few items with unique ID's.
3: Call the parent's add function to add all items to it. All objects will be
   added accordingly and the tree will keep sub dividing as necessary.
4: A few helper functions exist to display output. Refer to main.cpp for more
   information.

   
Thanks

Duncan Camilleri
