//
// Monza (MI), Italy
// 6 september 2005
// Isidoro Ghezzi
// isidoro.please_no_spam1234.ghezzi@tiscali.it
//
// Lissone (MB), Italy
// Sat Jun 14 16:12:24 CEST 2014
// Isidoro Ghezzi
// isidoro.ghezzi@icloud.com
//
// Ported to mavericks Mac OS X 10.9
//

#include <iostream>
#include <math.h>
#include <algorithm>
#include <functional>
#include <stdexcept>
#include <ctime>
#include <fstream>
#include <sstream>
#include <vector>

namespace isi{
	namespace prv{
		template <class TObject>
		void Dump (const char * theFunctionName, const char * theExpression, const TObject &theValue){
			std::cout << theFunctionName << ": " << theExpression << ": " << theValue << ";" << std::endl;
		}
	}

	template <class TObject>
	void DumpMacro (const char * theIdentifier, const TObject &theValue){
		std::cout << theIdentifier << ": " << theValue << ";" << std::endl;
	}
	
	#define ISI_MK_STR(theExpression) (#theExpression)
	
	#ifdef __GNUC__
	#ifdef _DEBUG
	#define ISI_DUMP(theExpression) prv::Dump (__PRETTY_FUNCTION__, ISI_MK_STR(theExpression), (theExpression))
	#else
	#define ISI_DUMP(theExpression) prv::Dump (__func__, ISI_MK_STR(theExpression), (theExpression))
	#endif
	#else
	#define ISI_DUMP(theExpression) prv::Dump ("noname", ISI_MK_STR(theExpression), (theExpression))
	#endif
}
namespace ig{
#ifdef IG_DIM_OF_ARRAY
#error "IG_DIM_OF_ARRAY already defined"
#endif

#define IG_DIM_OF_ARRAY(theArray) (sizeof((theArray))/sizeof(*(theArray)))
}

namespace isi{
	static void HandleStdException (const std::exception &theException){
		ISI_DUMP (theException.what ());
	}

	static void HandleUnknownException (void){
		ISI_DUMP (__FILE__);
	}

	template <class TException>
	void PrivateAssert (const char * theFunctionName, const char * theExpression, const bool theAssertion, const char * theFileName, const int theLine){
		if (false == theAssertion){
			ISI_DUMP (theExpression);
			ISI_DUMP (theFileName);
			ISI_DUMP (theLine);
			ISI_DUMP (theFunctionName);
			throw TException (theExpression);
		}
	}

	#ifdef __GNUC__
	#define RuntimeAssert(theAssertion) PrivateAssert <std::runtime_error> (__PRETTY_FUNCTION__, ISI_MK_STR (theAssertion), (theAssertion), __FILE__, __LINE__)
	#else
	#define RuntimeAssert(theAssertion) PrivateAssert <std::runtime_error> ("noname", ISI_MK_STR (theAssertion), (theAssertion), __FILE__, __LINE__)
	#endif
	
	#ifdef _DEBUG
	#ifdef __GNUC__
	#define LogicAssert(theAssertion) PrivateAssert <std::logic_error> (__PRETTY_FUNCTION__, ISI_MK_STR (theAssertion), (theAssertion), __FILE__, __LINE__)
	#else
	#define LogicAssert(theAssertion) PrivateAssert <std::logic_error> ("noname", ISI_MK_STR (theAssertion), (theAssertion), __FILE__, __LINE__)
	#endif
	#else
	static inline void DoNothing (void){
	}
	#define LogicAssert(theAssertion)  DoNothing ()
	#endif
}

namespace isi{
	template <class TObject>
	bool IsGoodPtr (const TObject * theObject){
		return NULL != theObject;
	}

	static const char * BoolToStr (const bool theBool){
		const char * kVett []={
			"false",
			"true"
		};

		return kVett [theBool];
	}

	static bool IsDebugVersion (void){
		#ifdef _DEBUG
		return true;
		#else
		return false;
		#endif
	}
	
	static double GetGnuVersion (void){
		#ifdef __GNUC__
		return __GNUC__;
		#else
		return 0;
		#endif
	}
}

namespace isi{
    template <typename TNumeric>
    bool IsInCRange (const TNumeric theMin, const TNumeric theValue, const TNumeric theSup){
        LogicAssert(theSup > theMin);
        return (theValue >= theMin) && (theValue < theSup);
    }
}

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
		
		private:
		bool IsGoodData (void) const;

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
        ISI_DUMP(this);
	}

	SCell::~SCell (void){
	}

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
	typedef SCell * MatrixOfCellPtr [kDim][kDim];
	static void DumpMatrix (const char * theIdentifier, MatrixOfCellPtr  &theMatrix){
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
		bool operator () (Cell * theIter) const{
			LogicAssert (true == IsGoodPtr (theIter));
			return theIter->GetValue () == fCellValue;
		}

		SFindHelper (const CellValue theCellValue):
		fCellValue (theCellValue){
			LogicAssert ((theCellValue > 0) && (theCellValue <= 9));
		}
	};

	static bool IsAvailable (Cell * theCellVector [kDim], const CellValue theCellValue){
		LogicAssert ((theCellValue >= 1) && (theCellValue <= 9));

		Cell ** aIterResult = std::find_if (&theCellVector[0], &theCellVector[0] + kDim , SFindHelper (theCellValue));
		return  &theCellVector [kDim] == aIterResult;
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
	
	typedef Cell * CellVector [kDim];
	
	static bool IsGoodSchemaSequence (const CellVector &theCellVector){
		size_t aNumTotal = std::count_if (&theCellVector [0], &theCellVector[0] + kDim , SCountHelper (0));
		bool rit = true;
		for (CellValue aCellValue = 0; aCellValue < kDim && rit; ++aCellValue){
			const size_t aNumValue = std::count_if (&theCellVector [0], &theCellVector[0] + kDim , SCountHelper (aCellValue + 1));
			aNumTotal += aNumValue;
			rit = ((1 == aNumValue) || (0 == aNumValue));
		}
		return (kDim == aNumTotal) && rit;
	}
	
	static bool IsGoodSolutionSequence (const CellVector &theCellVector){
		LogicAssert (true == IsGoodSchemaSequence (theCellVector));
		bool rit = true;
		for (CellValue aCellValue = 0; aCellValue < kDim && rit; ++aCellValue){
			const size_t aNumValue = std::count_if (&theCellVector [0], &theCellVector[0] + kDim , SCountHelper (aCellValue + 1));
			rit = (1 == aNumValue);
		}
		return rit;
	}
	
	template <class TAlgo>
	bool IsGood ( MatrixOfCellPtr  &theRowVector,   MatrixOfCellPtr  &theColVector,   MatrixOfCellPtr  &theSqrVector, const TAlgo theAlgo){
		bool rit = true;
		for (std::size_t i = 0; (i < kDim) && rit; ++i){
			rit = theAlgo (theRowVector [i]) &&
				theAlgo (theColVector [i]) &&
				theAlgo (theSqrVector [i]);
		}
		return rit;		
	}
	
	/*
	static bool IsGoodSolution (MatrixOfCellPtr  &theRowVector,  MatrixOfCellPtr  &theColVector,  MatrixOfCellPtr  &theSqrVector){
		bool rit = true;
		for (std::size_t i = 0; i < kDim && rit; ++i){
			rit = IsGoodSolutionSequence (theRowVector [i]) &&
				IsGoodSolutionSequence (theColVector [i]) &&
				IsGoodSolutionSequence (theSqrVector [i]);
		}
		return rit;
	}

	static bool IsGoodSchema (MatrixOfCellPtr  &theRowVector,  MatrixOfCellPtr  &theColVector,  MatrixOfCellPtr  &theSqrVector){
		bool rit = true;
		for (std::size_t i = 0; i < kDim && rit; ++i){
			// ISI_DUMP (i);
			rit = IsGoodSchemaSequence (theRowVector [i]) &&
				IsGoodSchemaSequence (theColVector [i]) &&
				IsGoodSchemaSequence (theSqrVector [i]);
		}
		return rit;
	}
	*/
	
	static bool IsGoodSolution (MatrixOfCellPtr  &theRowVector,  MatrixOfCellPtr  &theColVector,  MatrixOfCellPtr  &theSqrVector){
		return IsGood (theRowVector, theColVector, theSqrVector, IsGoodSolutionSequence);
	}

	static bool IsGoodSchema (MatrixOfCellPtr  &theRowVector,  MatrixOfCellPtr  &theColVector,  MatrixOfCellPtr  &theSqrVector){
		return IsGood (theRowVector, theColVector, theSqrVector, IsGoodSchemaSequence);
	}
	
	const std::size_t kIndexHelper [81][3]={
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
		return std::count_if (theCellVector, theCellVector + kDim, MyCountFree);
	}
	
	static std::size_t CountFreeFree (const CellVector theCellVector1, const CellVector theCellVector2, const CellVector theCellVector3){
		bool aValueVect [kDim] = {false};
		for (int i = 0; i < kDim; ++i){
			const unsigned short aVal1 = theCellVector1 [i]->GetValue ();
			if (aVal1 > 0){
                LogicAssert(IsInCRange <std::size_t>(0, aVal1 - 1, IG_DIM_OF_ARRAY(aValueVect)));
				if (false == aValueVect [aVal1 - 1]){
					aValueVect [aVal1 - 1] = true;
				}
			}

			const unsigned short aVal2 = theCellVector2 [i]->GetValue ();
			if (aVal2 > 0){
                LogicAssert(IsInCRange <std::size_t>(0, aVal2 - 1, IG_DIM_OF_ARRAY(aValueVect)));
				if (false == aValueVect [aVal2 - 1]){
					aValueVect [aVal2 - 1] = true;
				}
			}

			const unsigned short aVal3 = theCellVector3 [i]->GetValue ();
			if (aVal3 > 0){
                LogicAssert(IsInCRange <std::size_t>(0, aVal3 - 1, IG_DIM_OF_ARRAY(aValueVect)));
				if (false == aValueVect [aVal3 - 1]){
					aValueVect [aVal3 - 1] = true;
				}
			}
		}
		
		return std::count (aValueVect, aValueVect + kDim, false);
	}
	
	static unsigned short TransformIndex (const unsigned short theIndex, const Cell * theCell, const MatrixOfCellPtr &theRowVector, const MatrixOfCellPtr &theColVector, const MatrixOfCellPtr &theSqrVector){
		LogicAssert (true == IsGoodPtr (theCell));
		static std::vector <std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > > sTransformArray;
		static bool sFirstTime = true;
		if (sFirstTime){
			sFirstTime = false;
			for (unsigned short i = 0; i < kDim * kDim; ++i){
				const std::size_t aRowIndex = kIndexHelper [i][0] - 1;
				const std::size_t aColIndex = kIndexHelper [i][1] - 1;
				const std::size_t aSqrIndex = kIndexHelper [i][2] - 1;
				const std::size_t aCountFreeRow = CountFree (theRowVector [aRowIndex]);
				const std::size_t aCountFreeCol = CountFree (theColVector [aColIndex]);
				const std::size_t aCountFreeSqr = CountFree (theSqrVector [aSqrIndex]);
				const std::size_t aCountFreeFree = CountFreeFree (theRowVector [aRowIndex], theColVector [aColIndex], theSqrVector [aSqrIndex]);
				const std::pair <unsigned short, std::pair <ECellStatus, unsigned short> > aPair (i, std::pair <ECellStatus, unsigned short> (theCell [i].GetStatus (), aCountFreeFree));
				sTransformArray.push_back (aPair);
			}
			std::sort (sTransformArray.begin (), sTransformArray.end (), MyCompare);
			std::for_each (sTransformArray.begin (), sTransformArray.end (), MyDump);
		}
		// return  kDim*kDim - theIndex - 1;
		return sTransformArray [theIndex].first;
	}
	
	static bool HandleNextRecursive (const unsigned short theIndex, Cell * theCell, MatrixOfCellPtr &theRowVector, MatrixOfCellPtr &theColVector, MatrixOfCellPtr &theSqrVector){
		LogicAssert (true == IsGoodPtr (theCell));
		LogicAssert (true == IsGoodSchema (theRowVector, theColVector, theSqrVector));
		// DumpMatrix ("theRowVector", theRowVector);
		const unsigned short aTransformedIndex = TransformIndex (theIndex, theCell, theRowVector, theColVector, theSqrVector);
		gNumOfCall++;
		if (theIndex >= kDim * kDim){
			++gNumOfSolution;
			if (!(gNumOfSolution % 100000) || true){
				const unsigned long aCurrentSolution = gNumOfSolution;
				ISI_DUMP (aCurrentSolution);
				ISI_DUMP (gNumOfCall);
				ISI_DUMP (static_cast <double >(gNumOfCall)/aCurrentSolution);
				DumpMatrix ("theRowVector", theRowVector);
				ISI_DUMP (BoolToStr (IsGoodSolution (theRowVector, theColVector, theSqrVector)));
			}
			return false;
		}

		LogicAssert (/*(theIndex >= 0) &&*/ (theIndex < kDim * kDim));
		
		if (theCell [aTransformedIndex].GetStatus () == eCellStatusConstantInputInitValue){
			return HandleNextRecursive (NextIndex (theIndex), theCell, theRowVector, theColVector, theSqrVector); 
		}

		bool rit = false;
		for (CellValue aValue = 0; aValue < kDim; ++aValue){
			// if ((theCell [theIndex].GetValue () == aValue+1) /*|| (theCell [theIndex].GetValue () == 0)*/) continue;
			LogicAssert (theCell [aTransformedIndex].GetValue () != aValue+1);
			LogicAssert (theCell [aTransformedIndex].GetValue () == 0);
			
			#ifdef _DEBUG
			const std::size_t aRowIndex = kIndexHelper [aTransformedIndex][0] - 1; const bool aRowAvailable = isi::IsAvailable (theRowVector [aRowIndex], aValue + 1);
			const std::size_t aColIndex = kIndexHelper [aTransformedIndex][1] - 1; const bool aColAvailable = isi::IsAvailable (theColVector [aColIndex], aValue + 1);
			const std::size_t aSqrIndex = kIndexHelper [aTransformedIndex][2] - 1; const bool aSqrAvailable = isi::IsAvailable (theSqrVector [aSqrIndex], aValue + 1);
			const bool aGood = aRowAvailable && aColAvailable && aSqrAvailable;
			#else
			const std::size_t aRowIndex = kIndexHelper [aTransformedIndex][0] - 1;
			const std::size_t aColIndex = kIndexHelper [aTransformedIndex][1] - 1;
			const std::size_t aSqrIndex = kIndexHelper [aTransformedIndex][2] - 1;
			const bool aGood = isi::IsAvailable (theRowVector [aRowIndex], aValue + 1) &&
				isi::IsAvailable (theColVector [aColIndex], aValue + 1) &&
				isi::IsAvailable (theSqrVector [aSqrIndex], aValue + 1);
			#endif
			if (aGood){
				theCell [aTransformedIndex].SetValue (aValue + 1);
				rit = HandleNextRecursive (NextIndex (theIndex), theCell, theRowVector, theColVector, theSqrVector); 
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
		std::string aLineString;
		while (std::getline (aInputFileStrem, aLineString)){
			aFileString += aLineString;
		}
		}
		// ISI_DUMP (aFileString);
		std::istringstream   incoming_stream(aFileString);
		for (int i = 0; i < kDim*kDim; ++i){
			CellValue aCellValue;
			std::string aSeparetor;
			incoming_stream >> aCellValue;
			incoming_stream >> aSeparetor;
			theCellValue [i] = aCellValue;
			// ISI_DUMP (aCellValue);
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
		// return;
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
		Cell * aRow  [kDim][kDim]= {{NULL}};
		std::size_t i = 0;
		std::size_t j = 0;
		for (j = 0; j < kDim; ++j){
			for (i = 0; i < kDim; ++i){
                LogicAssert(IsInCRange <std::size_t>(0, i + j*kDim, IG_DIM_OF_ARRAY(aCell)));
				aRow [j][i] = &aCell [i + j*kDim];
			}
		}
		
		isi::DumpMatrix ("aRow", aRow);
		
		// alias of columns
		Cell * aCol  [kDim][kDim] = {{NULL}};
		for (j = 0; j < kDim; ++j){
			for (i = 0; i < kDim; ++i){
				aCol [i][j] = &aCell [i + j*kDim];
			}
		}
		
		// alias of square
		Cell * aSqr [kDim][kDim] = {{NULL}};
		const int aTransform [kDim][3] = {
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
					aSqr [j][aIndex] = aRow [a] [b];
					++aIndex;	
				}
			}
		}
		isi::DumpMatrix ("aRow", aRow);
		isi::DumpMatrix ("aCol", aCol);
		isi::DumpMatrix ("aSqr", aSqr);
		
		RuntimeAssert (true == IsGoodSchema (aRow, aCol, aSqr));
		(void) isi::HandleNextRecursive (0, aCell, aRow, aCol, aSqr);

		isi::ISI_DUMP (isi::gNumOfCall);
		isi::ISI_DUMP (isi::gNumOfSolution);

	}
}

int main (int argc, char* argv []){
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
