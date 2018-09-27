CC = g++
CFLAGS = -Wall -ansi -pedantic -Wextra
EXECUTABLE = sudoku

all: release debug

debug: Makefile main.cpp common.h
	$(CC) $(CFLAGS) -std=c++11 -g -D_DEBUG main.cpp -o $(EXECUTABLE)_d.out

release: Makefile main.cpp common.h
	$(CC) $(CFLAGS) -std=c++11 -O3 main.cpp -o $(EXECUTABLE).out

.PHONY: clean
clean:
	-rm -f *.out
	-rm -rf *.dSYM
