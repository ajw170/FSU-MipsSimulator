CC= gcc
CXX= g++ 

all: clean sim.exe

.c.o:
	$(CC) -g -O0 -c -o $@ $<
.cpp.o:
	$(CXX) -g -O0 -c -o $@ $<  -std=c++11

sim.exe: sim.o
	$(CXX) -o sim.exe sim.o -std=c++11

.PHONY: clean

clean:
	rm -f log.txt *.o *~ \#*\#
