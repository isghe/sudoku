CC = g++
CFLAGS = -Wall -ansi -pedantic -Wextra
EXECUTABLE = sudoku

all: release debug

debug:
	$(CC) $(CFLAGS) -g -D_DEBUG main.cpp -o $(EXECUTABLE)_d.out

release:
	$(CC) $(CFLAGS) -O3 main.cpp -o $(EXECUTABLE).out

.PHONY: clean
clean:
	-rm -f *.out
