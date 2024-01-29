CPP = g++

CFLAGS = -Wall -std=c++23 -O3 -g3 -D_GNU_SOURCE
CXXFLAGS = $(CFLAGS)
LDFLAGS = -lglfw -lrt -lm -ldl -lfmt

#.PHONY: test clean
O_FILES = termctl.o xdg.o main.o

bsh: $(O_FILES)
	$(CPP) $(CXXFLAGS) -o bsh $(O_FILES) $(LDFLAGS)

clean:
	rm -f main.o bsh

termctl.o: termctl.cc termctl.h
xdg.o: xdg.cc xdg.h
main.o: main.cc
