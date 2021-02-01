sudoku
======

C++ Sudoku solver

Created in 2005, ported to Mavericks Mac OS X 10.9, Yosemite OS X 10.10.3, OS X El Capitan 10.11.6, macOS High Sierra 10.13.4, macOS Catalina 10.15.7.

to compile:

`$ make`

to run:

`$ ./sudoku.out example/sample1.sdk`

Example from [Solve Diabolical Sudoku Puzzles - Very Hard](https://www.youtube.com/watch?v=oPTe52OmMEk)

```
$ ./sudoku.out example/diabolical_very_hard.sdk
main: isi::GetGnuVersion (): 4;
__FILE__: main.cpp;
__DATE__: Sep 27 2018;
__TIME__: 18:32:38;
main: isi::BoolToStr (isi::IsDebugVersion ()): false;
my_main: theFileName: example/diabolical_very_hard.sdk;
my_main: std::count_if (aCell, aCell + kDim * kDim, IsInitValue): 26;
aRow:
	0, 	7, 	0, 	2, 	5, 	0, 	4, 	0, 	0, 
	8, 	0, 	0, 	0, 	0, 	0, 	9, 	0, 	3, 
	0, 	0, 	0, 	0, 	0, 	3, 	0, 	7, 	0, 
	7, 	0, 	0, 	0, 	0, 	4, 	0, 	2, 	0, 
	1, 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	7, 
	0, 	4, 	0, 	5, 	0, 	0, 	0, 	0, 	8, 
	0, 	9, 	0, 	6, 	0, 	0, 	0, 	0, 	0, 
	4, 	0, 	1, 	0, 	0, 	0, 	0, 	0, 	5, 
	0, 	0, 	7, 	0, 	8, 	2, 	0, 	3, 	0, 
HandleNextRecursive: aCurrentSolution: 1;
HandleNextRecursive: gNumOfCall: 15628;
HandleNextRecursive: static_cast <double >(gNumOfCall)/aCurrentSolution: 15628;
theRowVector:
	9, 	7, 	3, 	2, 	5, 	8, 	4, 	1, 	6, 
	8, 	2, 	4, 	1, 	6, 	7, 	9, 	5, 	3, 
	6, 	1, 	5, 	9, 	4, 	3, 	8, 	7, 	2, 
	7, 	5, 	8, 	3, 	9, 	4, 	6, 	2, 	1, 
	1, 	3, 	9, 	8, 	2, 	6, 	5, 	4, 	7, 
	2, 	4, 	6, 	5, 	7, 	1, 	3, 	9, 	8, 
	3, 	9, 	2, 	6, 	1, 	5, 	7, 	8, 	4, 
	4, 	8, 	1, 	7, 	3, 	9, 	2, 	6, 	5, 
	5, 	6, 	7, 	4, 	8, 	2, 	1, 	3, 	9, 
HandleNextRecursive: BoolToStr (IsGoodSolution (theSchema)): true;
my_main: isi::gNumOfCall: 31775;
my_main: isi::gNumOfSolution: 1;
main: rit: 0;
main: static_cast <double> ((std::clock () - aStart))/1000000: 0.004447;
```

solved in 0.004447 seconds

isidoro ghezzi

;-)
