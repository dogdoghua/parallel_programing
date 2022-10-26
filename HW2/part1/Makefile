CC = gcc
CXXFLAGS = -I./common -O3 -std=c++17 -Wall -lpthread

all: pi.out

pi.o: pi.c 
	$(CXX) $(CXXFLAGS) -c pi.c 

pi.out: pi.o
	$(CXX) $(CXXFLAGS) pi.o -o pi.out

clean:
	rm -f pi.o pi.out pi
