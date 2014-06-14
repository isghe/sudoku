sudoku
======

C++ Sudoku solver

Created in 2005, ported in 2014 on Mavericks Mac OS X 10.9

to compile:
$ g++ -O3 -Wall -ansi -pedantic main.cpp -o sudoku.out

to run:
$ ./sudoku.out example/sample1.sdk

main: isi::GetGnuVersion (): 4;
__FILE__: main.cpp;
__DATE__: Jun 14 2014;
__TIME__: 17:50:14;
main: isi::BoolToStr (isi::IsDebugVersion ()): false;
my_main: theFileName: example/sample1.sdk;
my_main: std::count_if (aCell, aCell + kDim * kDim, IsInitValue): 34;
aRow:
	0, 	0, 	6, 	0, 	5, 	0, 	8, 	0, 	2, 
	0, 	0, 	8, 	0, 	0, 	4, 	5, 	0, 	9, 
	0, 	5, 	7, 	0, 	8, 	2, 	3, 	0, 	0, 
	1, 	0, 	0, 	0, 	6, 	0, 	0, 	5, 	0, 
	0, 	8, 	5, 	0, 	0, 	3, 	0, 	2, 	6, 
	6, 	4, 	0, 	0, 	0, 	0, 	0, 	0, 	7, 
	0, 	0, 	9, 	1, 	0, 	0, 	6, 	0, 	0, 
	5, 	0, 	1, 	8, 	0, 	0, 	2, 	0, 	0, 
	0, 	0, 	4, 	0, 	3, 	0, 	7, 	0, 	0, 
HandleNextRecursive: aCurrentSolution: 1;
HandleNextRecursive: gNumOfCall: 141210;
HandleNextRecursive: static_cast <double >(gNumOfCall)/aCurrentSolution: 141210;
theRowVector:
	9, 	1, 	6, 	3, 	5, 	7, 	8, 	4, 	2, 
	3, 	2, 	8, 	6, 	1, 	4, 	5, 	7, 	9, 
	4, 	5, 	7, 	9, 	8, 	2, 	3, 	6, 	1, 
	1, 	9, 	2, 	7, 	6, 	8, 	4, 	5, 	3, 
	7, 	8, 	5, 	4, 	9, 	3, 	1, 	2, 	6, 
	6, 	4, 	3, 	5, 	2, 	1, 	9, 	8, 	7, 
	2, 	7, 	9, 	1, 	4, 	5, 	6, 	3, 	8, 
	5, 	3, 	1, 	8, 	7, 	6, 	2, 	9, 	4, 
	8, 	6, 	4, 	2, 	3, 	9, 	7, 	1, 	5, 
HandleNextRecursive: BoolToStr (IsGoodSolution (theRowVector, theColVector, theSqrVector)): true;
my_main: isi::gNumOfCall: 259830;
my_main: isi::gNumOfSolution: 1;
main: rit: 0;
main: std::clock () - aStart: 24399;

Have fun!

isidoro ghezzi
;-)
