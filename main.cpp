//
// Monza (MI), Italy
// 6 september 2005
// Copyright 2005, Isidoro Ghezzi
// isidoro.ghezzi@tiscali.it
//
// Lissone (MB), Italy
// Sat Jun 14 16:12:24 CEST 2014
// Copyright 2005-2015, Isidoro Ghezzi
// isidoro.ghezzi@icloud.com
//
// Porting to:
// 	mavericks, Mac OS X 10.9
//	yosemite, OS X 10.10.3
//

#include "common.h"

#include <math.h>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>

namespace isi{
	typedef unsigned short CellValue;
	
	typedef enum{
		eCellStatusConstantInputInitValue,
		eCellStatusVariable
	}
	ECellStatus;
	typedef ECellStatus CellStatus;

	struct SCell{
		private:
		CellValue fValue;
		CellStatus fStatus; // 0: variable, 1: constant init input value;

		#ifdef _DEBUG
		private:
		bool IsGoodData (void) const;
		#endif
		
		public:
		void SetInitValue (const CellValue theValue);

		public:
		CellValue GetValue (void) const;
		void SetValue (const CellValue theValue);

		public:
		CellStatus GetStatus (void) const;

		public:
		SCell (void);
		virtual ~SCell (void);
	};

	SCell::SCell (void):
	fValue (0),
	fStatus (eCellStatusVariable){
	}

	SCell::~SCell (void){
	}

	#ifdef _DEBUG
	bool SCell::IsGoodData (void) const{
		const bool ret =  ((eCellStatusConstantInputInitValue == fStatus) && ((fValue > 0) && (fValue <=9)))
			||
			((eCellStatusVariable == fStatus)  && (/* (fValue >= 0) &&*/ (fValue <=9)));
		if (false == ret){
			ISI_DUMP (fStatus);
			ISI_DUMP (fValue);
		}
		return ret;
	}
	#endif

	CellValue SCell::GetValue (void) const{
		LogicAssert (true == this->IsGoodData ());
		return fValue;
	}

	void SCell::SetValue (const CellValue theValue){
		LogicAssert (eCellStatusVariable == fStatus);
		fValue = theValue;
		LogicAssert (true == this->IsGoodData ());
	}

	CellStatus SCell::GetStatus (void) const{
		LogicAssert (true == this->IsGoodData ());
		return fStatus;
	}

	void SCell::SetInitValue (const CellValue theValue){
		LogicAssert ((theValue > 0) && (theValue <=9));
		LogicAssert (fValue != theValue);
		fValue = theValue;
		fStatus = eCellStatusConstantInputInitValue;
	}

	typedef SCell Cell;
	const std::size_t kDim = 9;
}

namespace isi{
	typedef std::vector <Cell *> CellVector; // 376616
	typedef std::vector <CellVector> MatrixOfCellPtr; // 352973
	static void DumpMatrix (const char * theIdentifier, const MatrixOfCellPtr  &theMatrix){
		LogicAssert (true == IsGoodPtr (theIdentifier));
		std::cout << theIdentifier << ":" << std::endl;
		for (std::size_t j = 0; j < kDim; ++j){
			for (std::size_t i = 0; i < kDim; ++i){
				std::cout << "\t" /*<< "[" << theMatrix [j][i]->fStatus << "] "*/<< theMatrix [j][i]->GetValue () << ", ";
				// std::cout << "\t" /*<< "[" << theMatrix [j][i]->fStatus << "] "*/<< theMatrix [j][i] << ", ";
			}
			std::cout << std::endl;
		}
	}

	struct SFindHelper: public std::binary_function <bool, SCell *, CellValue>{
		const CellValue fCellValue;
		bool operator () (const Cell * theIter) const{
			LogicAssert (true == IsGoodPtr (theIter));
			return theIter->GetValue () == fCellValue;
		}

		SFindHelper (const CellValue theCellValue):
		fCellValue (theCellValue){
			LogicAssert ((theCellValue > 0) && (theCellValue <= 9));
		}
	};

	static bool IsAvailable (const CellVector &theCellVector, const CellValue theCellValue){
        LogicAssert ((theCellValue >= 1) && (theCellValue <= 9));

        const CellVector::const_iterator aIterResult = std::find_if (theCellVector.begin (), theCellVector.end () , SFindHelper (theCellValue));
        return  theCellVector.end () == aIterResult;
	}

	struct SCountHelper: public std::binary_function <bool, Cell *, CellValue>{
		const CellValue fCellValue;
		bool operator () (Cell * theIter) const{
			LogicAssert (true == IsGoodPtr (theIter));
			return theIter->GetValue () == fCellValue;
		}

		SCountHelper (const CellValue theCellValue):
		fCellValue (theCellValue){
			LogicAssert (/*(theCellValue >= 0) &&*/ (theCellValue <= 9));
		}
	};

	static bool IsGoodSchemaSequence (const CellVector &theCellVector){
		size_t aNumTotal = std::count_if (theCellVector.begin (), theCellVector.end (), SCountHelper (0));
		bool rit = true;
		for (CellValue aCellValue = 0; aCellValue < kDim && rit; ++aCellValue){
			const size_t aNumValue = std::count_if (theCellVector.begin (), theCellVector.end (), SCountHelper (aCellValue + 1));
			aNumTotal += aNumValue;
			rit = ((1 == aNumValue) || (0 == aNumValue));
		}
		return (kDim == aNumTotal) && rit;
	}
	
	static bool IsGoodSolutionSequence (const CellVector &theCellVector){
		LogicAssert (true == IsGoodSchemaSequence (theCellVector));
		bool rit = true;
		for (CellValue aCellValue = 0; aCellValue < kDim && rit; ++aCellValue){
			const size_t aNumValue = std::count_if (theCellVector.begin (), theCellVector.end () , SCountHelper (aCellValue + 1));
			rit = (1 == aNumValue);
		}
		return rit;
	}
	
	template <class TAlgo>
	bool IsGood (const MatrixOfCellPtr * theSchema, const TAlgo theAlgo){
        LogicAssert(true == IsGoodPtr(theSchema));
		bool rit = true;
		for (std::size_t i = 0; (i < kDim) && rit; ++i){
			rit = theAlgo (theSchema [0][i]) &&
				theAlgo (theSchema [1][i]) &&
				theAlgo (theSchema [2][i]);
		}
		return rit;		
	}
	
	static bool IsGoodSolution (const MatrixOfCellPtr * theSchema){
        LogicAssert(true == IsGoodPtr(theSchema));
		return IsGood (theSchema, IsGoodSolutionSequence);
	}

	static bool IsGoodSchema (const MatrixOfCellPtr * theSchema){
        LogicAssert(true == IsGoodPtr(theSchema));
		return IsGood (theSchema, IsGoodSchemaSequence);
	}
	
	const std::size_t kIndexHelper [81][3]={
    // it would be interesting automagically create this matrix at compile time, using pseudo programming
		{1, 1, 1}, {1, 2, 1}, {1, 3, 1}, {1, 4, 2}, {1, 5, 2}, {1, 6, 2}, {1, 7, 3}, {1, 8, 3}, {1, 9, 3},
		{2, 1, 1}, {2, 2, 1}, {2, 3, 1}, {2, 4, 2}, {2, 5, 2}, {2, 6, 2}, {2, 7, 3}, {2, 8, 3}, {2, 9, 3},
		{3, 1, 1}, {3, 2, 1}, {3, 3, 1}, {3, 4, 2}, {3, 5, 2}, {3, 6, 2}, {3, 7, 3}, {3, 8, 3}, {3, 9, 3},
		{4, 1, 4}, {4, 2, 4}, {4, 3, 4}, {4, 4, 5}, {4, 5, 5}, {4, 6, 5}, {4, 7, 6}, {4, 8, 6}, {4, 9, 6},
		{5, 1, 4}, {5, 2, 4}, {5, 3, 4}, {5, 4, 5}, {5, 5, 5}, {5, 6, 5}, {5, 7, 6}, {5, 8, 6}, {5, 9, 6},
		{6, 1, 4}, {6, 2, 4}, {6, 3, 4}, {6, 4, 5}, {6, 5, 5}, {6, 6, 5}, {6, 7, 6}, {6, 8, 6}, {6, 9, 6},
		{7, 1, 7}, {7, 2, 7}, {7, 3, 7}, {7, 4, 8}, {7, 5, 8}, {7, 6, 8}, {7, 7, 9}, {7, 8, 9}, {7, 9, 9},
		{8, 1, 7}, {8, 2, 7}, {8, 3, 7}, {8, 4, 8}, {8, 5, 8}, {8, 6, 8}, {8, 7, 9}, {8, 8, 9}, {8, 9, 9},
		{9, 1, 7}, {9, 2, 7}, {9, 3, 7}, {9, 4, 8}, {9, 5, 8}, {9, 6, 8}, {9, 7, 9}, {9, 8, 9}, {9, 9, 9}
	};
	
	static unsigned long gNumOfCall = 0;
	static unsigned long gNumOfSolution = 0;
	
	static unsigned short NextIndex (const unsigned short theIndex){
	// isghe: here i am simply choosing the next one,
	// but in real life you can choose a smarter one:
	// the exaustive result will not change but yuo'll reach the full solution in less (maybe also in time) recursive call.
		LogicAssert (/*(theIndex >= 0) &&*/ (theIndex <  kDim * kDim));
		
		// isghe: to_do (but you'll need to change the caller):
		// LogicAssert (/*(theIndex >= 0) &&*/ (theIndex + 1<  kDim * kDim));
		
		return theIndex + 1;
	}
	
	static int CalcSortValue (const std::pair <ECellStatus, unsigned short> &thePair){
		return thePair.first * 1000 + thePair.second;
	}
	
	static bool MyCompare (const std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > &thePair1,  const std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > &thePair2){
		return  CalcSortValue (thePair2.second) >  CalcSortValue (thePair1.second);
	}

#ifdef _DEBUG
	static bool MyDump (const std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > &thePair){
		ISI_DUMP (thePair.first);
		ISI_DUMP (thePair.second.first);
		ISI_DUMP (thePair.second.second);
		return true;
	}

	
	static bool MyCountFree (const Cell * theCell){
		return theCell->GetStatus () == eCellStatusVariable;
	}
	
	static std::size_t CountFree (const CellVector &theCellVector){
		return std::count_if (theCellVector.begin (), theCellVector.end (), MyCountFree);
	}
#endif

	static std::size_t CountFreeFree (CellVector ** aMatrix){
        LogicAssert(true == IsGoodPtr(aMatrix));
		bool aValueVect [kDim] = {false};
		for (std::size_t i = 0; i < kDim; ++i){
            for (int j = 0; j < 3; ++j){
                LogicAssert(true == IsGoodPtr(aMatrix[j]));
                LogicAssert(true == IsGoodPtr((*aMatrix[j])[i]));
                const unsigned short aVal = (*aMatrix[j])[i]->GetValue ();
                if (aVal > 0){
                    LogicAssert(IsInCRange <std::size_t>(0, aVal - 1, IG_DIM_OF_ARRAY(aValueVect)));
                    if (false == aValueVect [aVal - 1]){
                        aValueVect [aVal - 1] = true;
                    }
                }
            }
		}

		return std::count (aValueVect, aValueVect + kDim, false);
	}
	
	static unsigned short TransformIndex (const unsigned short theIndex, const Cell * theCell, MatrixOfCellPtr * theSchema){
		LogicAssert (true == IsGoodPtr (theCell));
		LogicAssert (true == IsGoodPtr (theSchema));
		static std::vector <std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > > sTransformArray;
		static bool sFirstTime = true;
		if (sFirstTime){
			sFirstTime = false;
			for (unsigned short i = 0; i < kDim * kDim; ++i){
				const std::size_t aRowIndex = kIndexHelper [i][0] - 1;
				const std::size_t aColIndex = kIndexHelper [i][1] - 1;
				const std::size_t aSqrIndex = kIndexHelper [i][2] - 1;
#ifdef _DEBUG
                const std::size_t aCountFreeRow = CountFree (theSchema [0][aRowIndex]);
                const std::size_t aCountFreeCol = CountFree (theSchema [1][aColIndex]);
                const std::size_t aCountFreeSqr = CountFree (theSchema [2][aSqrIndex]);
                ISI_DUMP(aCountFreeRow);
                ISI_DUMP(aCountFreeCol);
                ISI_DUMP(aCountFreeSqr);
#endif
                CellVector * aMatrix []={
                    &theSchema [0][aRowIndex], &theSchema [1][aColIndex], &theSchema [2][aSqrIndex]
                };
				const std::size_t aCountFreeFree = CountFreeFree (aMatrix);
				const std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > aPair (i, std::pair <ECellStatus, unsigned short> (theCell [i].GetStatus (), aCountFreeFree));
				sTransformArray.push_back (aPair);
			}
			std::sort (sTransformArray.begin (), sTransformArray.end (), MyCompare);
			#ifdef _DEBUG
			std::for_each (sTransformArray.begin (), sTransformArray.end (), MyDump);
			#endif
		}
		// return  kDim*kDim - theIndex - 1;
		return sTransformArray [theIndex].first;
	}
	
    static bool IsGoodSchemaAvailable (const MatrixOfCellPtr * theSchema, const CellValue theValue, const unsigned short theTransformedIndex){
		LogicAssert (true == IsGoodPtr (theSchema));
		LogicAssert (true == IsGoodSchema (theSchema));
#ifdef _DEBUG
        bool aGood = true;
        for (int i = 0; i < 3 && aGood; ++i){
            const std::size_t aIndex = kIndexHelper [theTransformedIndex][i] - 1;
            aGood = isi::IsAvailable (theSchema [i][aIndex], theValue + 1);
        }
        return aGood;
#else
        return isi::IsAvailable (theSchema [0][kIndexHelper [theTransformedIndex][0] - 1], theValue + 1)
        && isi::IsAvailable (theSchema [1][kIndexHelper [theTransformedIndex][1] - 1], theValue + 1)
        && isi::IsAvailable (theSchema [2][kIndexHelper [theTransformedIndex][2] - 1], theValue + 1);
#endif
    }

	static bool HandleNextRecursive (const unsigned short theIndex, Cell * theCell, MatrixOfCellPtr * theSchema){
		LogicAssert (true == IsGoodPtr (theCell));
		LogicAssert (true == IsGoodPtr (theSchema));
		LogicAssert (true == IsGoodSchema (theSchema));

		const unsigned short aTransformedIndex = TransformIndex (theIndex, theCell, theSchema);
		gNumOfCall++;
		if (theIndex >= kDim * kDim){
			++gNumOfSolution;
			if (!(gNumOfSolution % 100000) || true){
				const unsigned long aCurrentSolution = gNumOfSolution;
				ISI_DUMP (aCurrentSolution);
				ISI_DUMP (gNumOfCall);
				ISI_DUMP (static_cast <double >(gNumOfCall)/aCurrentSolution);
				DumpMatrix ("theRowVector", theSchema [0]);
				ISI_DUMP (BoolToStr (IsGoodSolution (theSchema)));
			}
			return false;
		}

		LogicAssert (/*(theIndex >= 0) &&*/ (theIndex < kDim * kDim));
		
		if (theCell [aTransformedIndex].GetStatus () == eCellStatusConstantInputInitValue){
			return HandleNextRecursive (NextIndex (theIndex), theCell, theSchema);
		}

		bool rit = false;
		for (CellValue aValue = 0; aValue < kDim; ++aValue){
			LogicAssert (theCell [aTransformedIndex].GetValue () != aValue+1);
			LogicAssert (theCell [aTransformedIndex].GetValue () == 0);

            const bool aGood = IsGoodSchemaAvailable (theSchema, aValue, aTransformedIndex);
			if (aGood){
				theCell [aTransformedIndex].SetValue (aValue + 1);
				rit = HandleNextRecursive (NextIndex (theIndex), theCell, theSchema);
				if (rit){
					break;
				}
				else{
					theCell [aTransformedIndex].SetValue (0);
				}
			}
		}
		return rit;
	}
}

namespace isi{
	static bool IsInitValue (const Cell &theCell){
		return theCell.GetStatus () == eCellStatusConstantInputInitValue;
	}
	
	static void LoadCellValuesFromFile (const char * theFileName, CellValue * theCellValue){
		LogicAssert (true == IsGoodPtr (theFileName));
		LogicAssert (std::strlen (theFileName) > 0);
		LogicAssert (true == IsGoodPtr (theCellValue));
		std::string aFileString; 
		{
			std::ifstream aInputFileStrem (theFileName);
            RuntimeAssert(!!aInputFileStrem);
			std::string aLineString;
			while (std::getline (aInputFileStrem, aLineString)){
				aFileString += " " + aLineString;
			}
		}
		#if _DEBUG
		ISI_DUMP (aFileString);
		#endif
		std::istringstream incoming_stream(aFileString);
		for (std::size_t i = 0; i < kDim*kDim; ++i){
			CellValue aCellValue;
			std::string aSeparetor;
			incoming_stream >> aCellValue;
			incoming_stream >> aSeparetor;
			theCellValue [i] = aCellValue;
			#if _DEBUG
			ISI_DUMP (aCellValue);
			#endif
		}
	}
	
	static void my_main (const char * theFileName){
		LogicAssert (true == IsGoodPtr (theFileName));
		LogicAssert (std::strlen (theFileName) > 0);
		ISI_DUMP (theFileName);

		#if 1
		CellValue kCellValue [kDim * kDim]={
			0
		};
		LoadCellValuesFromFile (theFileName, kCellValue);

        #else
		const CellValue kCellValue [kDim * kDim]={
			0, 0, 6, 0, 5, 0, 8, 0, 2,
			0, 0, 8, 0, 0, 4, 5, 0, 9,
			0, 5, 7, 0, 8, 2, 3, 0, 0,

			1, 0, 0, 0, 6, 0, 0, 5, 0,
			0, 8, 5, 0, 0, 3, 0, 2, 6,
			6, 4, 0, 0, 0, 0, 0, 0, 7,

			0, 0, 9, 1, 0, 0, 6, 0, 0,
			5, 0, 1, 8, 0, 0, 2, 0, 0,
			0, 0, 4, 0, 3, 0, 7, 0, 0
		};
		#endif
		Cell aCell [kDim * kDim];
		
		for (std::size_t aIter = 0; aIter < kDim * kDim; ++aIter){
			if (0 != kCellValue [aIter]){
                LogicAssert(IsInCRange <std::size_t>(0, aIter, IG_DIM_OF_ARRAY(kCellValue)));
                LogicAssert(IsInCRange <std::size_t>(0, aIter, IG_DIM_OF_ARRAY(aCell)));
				aCell [aIter].SetInitValue (kCellValue [aIter]);
			}
		}
		
		ISI_DUMP (std::count_if (aCell, aCell + kDim * kDim, IsInitValue));

		// alias of rows
		MatrixOfCellPtr aRow (kDim);
		std::size_t i = 0;
		std::size_t j = 0;
		for (j = 0; j < kDim; ++j){
			for (i = 0; i < kDim; ++i){
                LogicAssert(IsInCRange <std::size_t>(0, i + j*kDim, IG_DIM_OF_ARRAY(aCell)));
				aRow [j].push_back(&aCell [i + j*kDim]);
			}
		}
		
		isi::DumpMatrix ("aRow", aRow);
		
		// alias of columns
		MatrixOfCellPtr aCol (kDim);
		for (j = 0; j < kDim; ++j){
			for (i = 0; i < kDim; ++i){
                LogicAssert(IsInCRange <std::size_t>(0, i + j*kDim, IG_DIM_OF_ARRAY(aCell)));
				aCol [i].push_back(&aCell [i + j*kDim]);
			}
		}
		
		// alias of square
		MatrixOfCellPtr aSqr (kDim);
		const std::size_t aTransform [kDim][3] = {
        // it would be interesting automagically create this matrix at compile time, using pseudo programming
			{0, 0, 0}, {1, 0, 3}, {2, 0, 6},
			{3, 3, 0}, {4, 3, 3}, {5, 3, 6},
			{6, 6, 0}, {7, 6, 3}, {8, 6, 6}
		};

		for (j = 0; j < kDim; ++j){
			std::size_t aIndex = 0;
			for (std::size_t a = aTransform [j][1]; a <  aTransform [j][1] + 3; ++a){
				for (std::size_t b = aTransform [j][2]; b <  aTransform [j][2] + 3; ++b){
                    LogicAssert(IsInCRange <std::size_t>(0, a, kDim));
                    LogicAssert(IsInCRange <std::size_t>(0, b, kDim));
                    LogicAssert(IsInCRange <std::size_t>(0, j, kDim));
                    LogicAssert(IsInCRange <std::size_t>(0, aIndex, kDim));
					aSqr [j].push_back (aRow [a] [b]);
					++aIndex;	
				}
			}
		}

		#ifdef _DEBUG
		isi::DumpMatrix ("aCol", aCol);
		isi::DumpMatrix ("aSqr", aSqr);
		#endif

        MatrixOfCellPtr aSchema []={
            aRow, aCol, aSqr
        };
		RuntimeAssert (true == IsGoodSchema (aSchema));
		(void) isi::HandleNextRecursive (0, aCell, aSchema);

		isi::ISI_DUMP (isi::gNumOfCall);
		isi::ISI_DUMP (isi::gNumOfSolution);

	}
}

int main (int argc, char* argv []){
/*
	compile:

 $ uname -a
Darwin Isidoros-MacBook-Pro.local 14.3.0 Darwin Kernel Version 14.3.0: Mon Mar 23 11:59:05 PDT 2015; root:xnu-2782.20.48~5/RELEASE_X86_64 x86_64

 $ g++ --version
Configured with: --prefix=/Applications/Xcode.app/Contents/Developer/usr --with-gxx-include-dir=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/usr/include/c++/4.2.1
Apple LLVM version 6.1.0 (clang-602.0.53) (based on LLVM 3.6.0svn)
Target: x86_64-apple-darwin14.3.0
Thread model: posix

 $ make
g++ -Wall -ansi -pedantic -Wextra -O3 main.cpp -o sudoku.out
g++ -Wall -ansi -pedantic -Wextra -g -D_DEBUG main.cpp -o sudoku_d.out

 run:
 $ ./sudoku.out example/diabolikus_08062008.sdk
main: isi::GetGnuVersion (): 4;
__FILE__: main.cpp;
__DATE__: May 21 2015;
__TIME__: 18:50:56;
main: isi::BoolToStr (isi::IsDebugVersion ()): false;
my_main: theFileName: example/diabolikus_08062008.sdk;
my_main: std::count_if (aCell, aCell + kDim * kDim, IsInitValue): 26;
aRow:
	7, 	0, 	1, 	0, 	5, 	0, 	0, 	0, 	0, 
	8, 	4, 	0, 	1, 	9, 	0, 	0, 	7, 	0, 
	0, 	0, 	0, 	0, 	0, 	2, 	0, 	0, 	0, 
	6, 	8, 	0, 	0, 	0, 	0, 	0, 	1, 	0, 
	0, 	0, 	7, 	0, 	0, 	0, 	9, 	0, 	0, 
	0, 	1, 	0, 	0, 	0, 	0, 	0, 	4, 	3, 
	0, 	0, 	0, 	2, 	0, 	0, 	0, 	0, 	0, 
	0, 	7, 	0, 	0, 	8, 	4, 	0, 	9, 	5, 
	0, 	0, 	0, 	0, 	7, 	0, 	2, 	0, 	6, 
HandleNextRecursive: aCurrentSolution: 1;
HandleNextRecursive: gNumOfCall: 17604;
HandleNextRecursive: static_cast <double >(gNumOfCall)/aCurrentSolution: 17604;
theRowVector:
	7, 	2, 	1, 	3, 	5, 	8, 	4, 	6, 	9, 
	8, 	4, 	5, 	1, 	9, 	6, 	3, 	7, 	2, 
	3, 	9, 	6, 	7, 	4, 	2, 	8, 	5, 	1, 
	6, 	8, 	2, 	4, 	3, 	9, 	5, 	1, 	7, 
	4, 	3, 	7, 	5, 	6, 	1, 	9, 	2, 	8, 
	5, 	1, 	9, 	8, 	2, 	7, 	6, 	4, 	3, 
	9, 	6, 	8, 	2, 	1, 	5, 	7, 	3, 	4, 
	2, 	7, 	3, 	6, 	8, 	4, 	1, 	9, 	5, 
	1, 	5, 	4, 	9, 	7, 	3, 	2, 	8, 	6, 
HandleNextRecursive: BoolToStr (IsGoodSolution (theSchema)): true;
my_main: isi::gNumOfCall: 21669;
my_main: isi::gNumOfSolution: 1;
main: rit: 0;
main: std::clock () - aStart: 7473;
*/
	const std::clock_t aStart = std::clock ();
	volatile int rit = 1;
	try{
		isi::ISI_DUMP (isi::GetGnuVersion ());
		isi::DumpMacro ("__FILE__" , __FILE__);
		isi::DumpMacro ("__DATE__" , __DATE__);
		isi::DumpMacro ("__TIME__" , __TIME__);
		isi::ISI_DUMP (isi::BoolToStr (isi::IsDebugVersion ()));
		
		isi::RuntimeAssert (2 == argc);
		isi::RuntimeAssert (true == isi::IsGoodPtr (argv [1]));
		isi::RuntimeAssert (std::strlen (argv [1]) > 0);
		isi::my_main (argv [1]);
		rit = 0;
	}

	catch (const std::exception &theException){
		rit = 2;
		isi::HandleStdException (theException);
	}
	catch (...){
		rit = 3;
		isi::HandleUnknownException ();
	}
	isi::ISI_DUMP (rit);
	isi::ISI_DUMP (std::clock () - aStart);
	
	return rit;
}
