CPP = g++

CFLAGS = -Wall -std=c++23 -O3 -g3 -D_GNU_SOURCE
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lglfw -lrt -lm -ldl -lfmt

#.PHONY: test clean

bsh: termctl.o main.o
	$(CPP) $(CXXFLAGS) -o bsh termctl.o main.o $(LDFLAGS)

clean:
	rm -f main.o bsh

termctl.o: termctl.cc termctl.h
main.o: main.cc
