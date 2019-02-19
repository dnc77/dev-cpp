all : clean cycbuf_dbg cycbuf_rel

clean:
	rm -f cycbuf_dbg
	rm -f cycbuf_rel

cycbuf_dbg:
	clear
	gcc -lstdc++ -std=c++14 -g -I ./ -o cycbuf_dbg cycbuf.cpp main.cpp

cycbuf_rel:
	clear
	gcc -lstdc++ -std=c++14 -I ./ -o cycbuf_rel cycbuf.cpp main.cpp
