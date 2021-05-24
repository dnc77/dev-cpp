all: pcapsniff.cpp pcapsniff.h
	gcc -I/usr/local/include/pcap -L/usr/local/lib  pcapsniff.cpp http.cpp ./logger.a -lpcap -lz -pthread -o ../bin/pcapsniff
	strip ../bin/pcapsniff

clean:
	rm -f ../bin/pcapsniff
