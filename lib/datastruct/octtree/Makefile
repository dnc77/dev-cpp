all : clean octree_debug octree_rel

clean:
	rm -f octree_rel
	rm -f octree_debug

octree_debug:
	clear
	gcc -lstdc++ -g -o octree_debug octree.cpp main.cpp

octree_rel:
	clear
	gcc -lstdc++ -o octree_rel octree.cpp main.cpp

