all : clean becode_dbg becode_rel

clean:
	rm -f becode_dbg
	rm -f becode_rel

becode_dbg:
	clear
	gcc -lm -lpthread -lstdc++ -std=c++14 -g -I ./ -o becode_dbg becode.cpp main.cpp

becode_rel:
	clear
	gcc -lm -lpthread -lstdc++ -std=c++14 -I ./ -o becode_rel becode.cpp main.cpp
