CPP = g++

CFLAGS = -Wall -std=c++23 -O3 -g3 -D_GNU_SOURCE
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lglfw -lrt -lm -ldl -lfmt

#.PHONY: test clean

clean:
	rm -f main.o bsh

main.o: main.cc

bsh: main.o
	$(CPP) $(CXXFLAGS) -o bsh main.o $(LDFLAGS)
