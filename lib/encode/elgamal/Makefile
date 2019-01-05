all : clean elg_debug elg_rel

clean:
	rm -f elg_rel
	rm -f elg_debug

elg_debug:
	clear
	gcc -lstdc++ -g -o elg_debug main.cpp CElGamal.cpp

elg_rel:
	clear
	gcc -lstdc++ -o elg_rel main.cpp CElGamal.cpp
