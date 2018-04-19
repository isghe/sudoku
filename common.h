//
// Monza (MI), Italy
// 6 september 2005
// Copyright 2005, Isidoro Ghezzi
// isidoro.ghezzi@tiscali.it
//
// Lissone (MB), Italy
// Sat Jun 14 16:12:24 CEST 2014
// Copyright 2005-2018, Isidoro Ghezzi
// isidoro.ghezzi@icloud.com
//
// Porting to:
//  mavericks, Mac OS X 10.9
//  yosemite, OS X 10.10.3
//  OS X El Capitan 10.11.5
//  macOS High Sierra 10.13.4
//

#ifndef common_h
#define common_h

#include <iostream>

#ifndef __cplusplus
#error "C++ needed"
#endif

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
#endif
