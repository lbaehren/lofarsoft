#ifndef HFCOMMON_H
#define HFCOMMON_H

//=========================================================================================
// Common with hfget ....
//=========================================================================================


#define ERROR( txt ) ( cout << endl << "ERROR in file " << __FILE__ << " line " << __LINE__ << ": "  << txt << endl ) 
#define MESSAGE( txt ) ( cout << "[" << __FILE__ << "," << __LINE__ << "]: " << txt << endl )
#define PRINT( txt ) ( cout << txt << endl )
#define MSG MESSAGE

//------------------------------------------
//Determines the architecture: 32 vs 64 bit
//------------------------------------------
//In the following we will assume that pointers 
//and HIntegers (i.e. my ints) are of that size.
 
#if LONG_MAX > 2147483647
#define H_OS64BIT 1
#define H_OS32BIT 0
#define H_NATIVE_WORD_SIZE int64_t
#else 
#define H_OS32BIT 1
#define H_OS64BIT 0
#define H_NATIVE_WORD_SIZE int32_t
#endif

//-------------------------------------------

typedef int64_t longint; /* should be at least 64 bits */
typedef H_NATIVE_WORD_SIZE HIntPointer; /* This should be an integer that has the same length as a Pointer*/


typedef HIntPointer address ;
//This is used to identify each data field uniquely
typedef HIntPointer objectid;
//counts to keep track of whether an object was modified - 0 if not modified
typedef HIntPointer modval;


#define MAXINT INT_MAX


//Define Types of Data to be stored
//typedef intptr_t HInteger;
typedef HIntPointer HInteger;
typedef double HNumber;
typedef std::complex<HNumber> HComplex;
typedef bool HBool;
typedef std::string HString;
typedef void* HPointer;

//Define types that are being used for the data reader and the casa arrays
typedef double CasaNumber;
typedef casa::DComplex CasaComplex;
typedef int CasaInteger;
typedef casa::Record CasaRecord ;

#define CasaMatrix casa::Matrix
#define CasaArray casa::Array
#define CasaVector casa::Vector
#define IterValueType typename Iter::value_type

//For some reasons NULL is primarily interpreted as Integer by the
//compiler (e.g., when it needs to determine the type of templated functions).
//So, this forces NULL to really be a NULL pointer ...
#define Null_p reinterpret_cast<HPointer>(NULL)

#define IntAsPtr(INT) reinterpret_cast<HPointer>(INT)
#define PtrAsInt(PTR) reinterpret_cast<HInteger>(PTR)

#endif
