//Include-file for preprocessor functions used to generate different
//wrappers of the HF library

#ifndef HFPP_H
#define HFPP_H

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/facilities/expand.hpp>

//Defines datatypes to iterate wrappers over
#define HF_PP_NUMERIC_TYPES (HInteger)(HNumber)(HComplex)
#define HF_PP_STRING_TYPES (HString)
#define HF_PP_POINTER_TYPES (HPointer)
#define HF_PP_NON_NUMERIC_TYPES HF_PP_STRING_TYPES HF_PP_POINTER_TYPES 
#define HF_PP_ALL_TYPES HF_PP_NUMERIC_TYPES HF_PP_NON_NUMERIC_TYPES
#define HF_PP_ALL_PYTHONTYPES HF_PP_NUMERIC_TYPES HF_PP_STRING_TYPES


/*

Here is an example input template to generate wrappers for a function
//$FILENAME: HFILE=$SELF.h  // This the outputfile that the awk splitter will write source code to

#define HF_PP_FILETYPE CC  // Tell the preprocessor that this is a c++ source code file
//#define HF_PP_FILETYPE hFILE  // Tell the preprocessor that this is a header file
//#define HF_PP_FILETYPE hPYTHON  // Tell the preprocessor that this is a header file for Python expose

//$WRITE_TO HFILE START:
#define HF_PP_FUNCNAME TEST  //The Name of the function
#define HF_PP_FUNCTYPE void  //Return value type of function
#define HF_PP_NVECS 2 //Number of (input/output) vectors
#define HF_PP_NPAR 3 //Number of scalar parameters
#define HF_PP_PAR0 (var_a, HInteger, 128, "Length of a block") //Input Parameter
#define HF_PP_PAR1 (var_b, HInteger, 128, "Length of a block")
#define HF_PP_PAR2 (var_c, HInteger, 128, "Length of a block")

//Now define the c++ commands that are executed in order to call the underlying function
#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(STL),HF_PP_PAR_PARAMETERS)
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(CASA),HF_PP_PAR_PARAMETERS)

//HF_PP_VEC_WRAPPER_CODE_STL
//HF_PP_VEC_WRAPPER_CODE_CASA

//does something like

//return TEST( vec0.begin(),vec0.end() , vec1.begin(),vec1.end(), var_a , var_b , var_c)
//return TEST( vec0.cbegin(),vec0.cend() , vec1.cbegin(),vec1.cend(), var_a , var_b , var_c)


//$WRITE_TO HFILE: HF_PP_GENERATE_WRAPPERS_hFILE 
//Now undefine the definitions above
#include "hfpp-undef.cc"
//$WRITE_TO HFILE END --------------------------------------------------

*/

//This is a simple trick to test whether a specific variable is set to true or not at all (=false)
#define HF_PP_IS_SET_TESTHF_PP_FUNCTYPE_T 0
#define HF_PP_IS_SET_TEST1 1
#define HF_PP_IS_SET_TEST0 0
#define HF_PP_IS_SET(WAT) BOOST_PP_CAT(HF_PP_IS_SET_TEST,WAT)

//generate the name of the Nth vector (e.g. vec0 for N=0) 
#define HF_PP_VECTORNAME(NMAX,N,data) BOOST_PP_CAT(vec,N)
#define HF_PP_VECTORITERATOR_STL(NMAX,N,data) BOOST_PP_CAT(vec,N).begin(),BOOST_PP_CAT(vec,N).end()
#define HF_PP_VECTORITERATOR_CASA(NMAX,N,data) BOOST_PP_CAT(vec,N).cbegin(),BOOST_PP_CAT(vec,N).cend()

//definition of an STL vector of type T and number N=0,1,2... 
#define HF_PP_DEF_VECTOR_STL(NMAX,N,T) vector<T> & HF_PP_VECTORNAME(NMAX,N,T)
#define HF_PP_DEF_VECTOR_CASA(NMAX,N,T) casa::Vector<T> & HF_PP_VECTORNAME(NMAX,N,T)

//produce the c++ definitions of N vectors of type T using the container type VECTYPE (e.g., STL or CASA) 
#define HF_PP_DEF_VECTORLIST(T,N,VECTYPE) BOOST_PP_ENUM(N, BOOST_PP_CAT(HF_PP_DEF_VECTOR_,VECTYPE),T)
#define HF_PP_PAR_VECTORLIST(N) BOOST_PP_ENUM(N, HF_PP_VECTORNAME,T)
//HF_PP_DEF_VECTORLIST(T,2,STL) will call HF_PP_DEF_VECTOR_STL/CASA to give:
//  vector<T> & vec0, vector<T> & vec1

//Generates an argument list containing start and end iterators to the input vectors
#define HF_PP_PAR_VECTORITERATORS(VECTYPE) BOOST_PP_ENUM(HF_PP_NVECS, BOOST_PP_CAT(HF_PP_VECTORITERATOR_,VECTYPE),T)
/*
#define HF_PP_NVECS 2
HF_PP_PAR_VECTORITERATORS(STL)
HF_PP_PAR_VECTORITERATORS(CASA)

will give 

 vec0.begin(),vec0.end() , vec1.begin(),vec1.end()
 vec0.cbegin(),vec0.cend() , vec1.cbegin(),vec1.cend()
*/



//Gives the c++ definition of the Nth (scalar) parameter
#define HF_PP_DEF_PARAMETER_N(NMAX,N,T) BOOST_PP_TUPLE_ELEM(4,1,BOOST_PP_CAT(HF_PP_PAR,N)) BOOST_PP_TUPLE_ELEM(4,0,BOOST_PP_CAT(HF_PP_PAR,N)) 
#define HF_PP_PAR_PARAMETER_N(NMAX,N,T) BOOST_PP_TUPLE_ELEM(4,0,BOOST_PP_CAT(HF_PP_PAR,N)) 

//Gives a list of the c++ definitions of all (i.e. N=NPAR) scalar parameters
#define HF_PP_DEF_PARAMETERS BOOST_PP_ENUM(HF_PP_NPAR,HF_PP_DEF_PARAMETER_N,T)
#define HF_PP_PAR_PARAMETERS BOOST_PP_ENUM(HF_PP_NPAR,HF_PP_PAR_PARAMETER_N,T)
//Adds a preceding comma to the parameterlist if nonzero so that this can be concatenated with another parameterlist
#define HF_PP_PAR_PARAMETERS_COMMA BOOST_PP_COMMA_IF(HF_PP_NPAR) HF_PP_PAR_PARAMETERS

//Create the entire parameter list consisting of the defintions of vectors and the other parameters
//HF_PP_PAR_PARLIST will give the list without the type definition
#define HF_PP_DEF_PARLIST(T,VECTYPE) HF_PP_DEF_VECTORLIST(T,HF_PP_NVECS,VECTYPE) BOOST_PP_COMMA_IF(HF_PP_NPAR) HF_PP_DEF_PARAMETERS
#define HF_PP_PAR_PARLIST(T,VECTYPE) HF_PP_PAR_VECTORLIST(HF_PP_NVECS) HF_PP_PAR_PARAMETERS_COMMA
/*

HF_PP_DEF_PARLIST(T,STL)
HF_PP_PAR_PARLIST(T,STL)

Will produce:

 vector<T> & vec0 , vector<T> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c
 vec0 , vec1 , var_a , var_b , var_c
*/

/*
HF_PP_PYTHON_WRAPPER_CODE: Defines a pointer to the function FUNC with
parameters in HF_PP_PARLIST and template class T. The pointer is assigned
the pointer to the corresponding function and later be given to the
Python function that will call it. Through this assignment the
function will also be instantiated with type T!

We can also create dummy functions with HF_PP_PYTHON_WRAPPER_DUMMY_CODE
which creates just empty functions of name FUNC and class type T. This
is used to instantiate certain templates that are needed by the GUI
for formal reasons (i.e. typically HString and HPointer functions).

 */
//Definitions and initialization of the external variables which hold the pointer to the functions to be exposed to Python
#define HF_PP_GET_FUNCTYPE(TYPE) BOOST_PP_IF(HF_PP_IS_SET(HF_PP_FUNCTYPE_T),TYPE,HF_PP_FUNCTYPE)
#define HF_PP_PYTHON_WRAPPER_CODE(NIL,FUNC,TYPE) HF_PP_GET_FUNCTYPE(TYPE) (*BOOST_PP_CAT(FUNC,TYPE))(HF_PP_DEF_PARLIST(TYPE,STL)) = &FUNC; 
#define HF_PP_PYTHON_WRAPPER_DUMMY_CODE(NIL,FUNC,TYPE) HF_PP_GET_FUNCTYPE(TYPE) FUNC (HF_PP_DEF_PARLIST(TYPE,STL)) {}
#define HF_PP_MAKE_PYTHON_WRAPPERS(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_WRAPPER_CODE,FUNC,TYPE_LIST)
#define HF_PP_MAKE_PYTHON_WRAPPERS_DUMMY(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_WRAPPER_DUMMY_CODE,FUNC,TYPE_LIST)

#define HF_PP_PYTHON_EXPOSE_CODE(NIL,FUNC,TYPE) def(#FUNC,BOOST_PP_CAT(FUNC,TYPE));
#define HF_PP_PYTHON_EXPOSE BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_EXPOSE_CODE,HF_PP_FUNCNAME,HF_PP_ALL_PYTHONTYPES)

//Generate the defintions of the external variables which hold the pointer to the functions to be exposed to Python - to be included in the hFILE
#define HF_PP_PYTHON_WRAPPER_CODE_hFILE(NIL,FUNC,T) extern HF_PP_GET_FUNCTYPE(T) (*BOOST_PP_CAT(FUNC,T))(HF_PP_DEF_PARLIST(T,STL)); 
#define HF_PP_MAKE_PYTHON_WRAPPERS_hFILE BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_WRAPPER_CODE_hFILE,HF_PP_FUNCNAME,HF_PP_ALL_TYPES)

/*
HF_PP_MAKE_PYTHON_WRAPPERS_hFILE

 gives

extern void (*TESTHInteger)( vector<HInteger> & vec0 , vector<HInteger> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHNumber)( vector<HNumber> & vec0 , vector<HNumber> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHComplex)( vector<HComplex> & vec0 , vector<HComplex> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHPointer)( vector<HPointer> & vec0 , vector<HPointer> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHString)( vector<HString> & vec0 , vector<HString> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c);
*/


#define HF_PP_NUM_PYTHON_WRAPPERS \
  HF_PP_MAKE_PYTHON_WRAPPERS_DUMMY(HF_PP_FUNCNAME,HF_PP_NON_NUMERIC_TYPES) \
  HF_PP_MAKE_PYTHON_WRAPPERS(HF_PP_FUNCNAME,HF_PP_ALL_TYPES)

/*
HF_PP_NUM_PYTHON_WRAPPERS

Will provide all the wrappers needed for a python wrapper for numerical vectors

void (*TESTHInteger)( vector<HInteger> & vec0 , vector<HInteger> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) = &TEST; void (*TESTHNumber)( vector<HNumber> & vec0 , vector<HNumber> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) = &TEST; void (*TESTHComplex)( vector<HComplex> & vec0 , vector<HComplex> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) = &TEST; void TEST ( vector<HPointer> & vec0 , vector<HPointer> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) {} void TEST ( vector<HString> & vec0 , vector<HString> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) {}

*/

#define HF_PP_VEC_WRAPPER_CODE_STL_DEFAULT(FUNC) return FUNC(HF_PP_PAR_VECTORITERATORS(STL) HF_PP_PAR_PARAMETERS_COMMA)
#define HF_PP_VEC_WRAPPER_CODE_CASA_DEFAULT(FUNC) return FUNC(HF_PP_PAR_VECTORITERATORS(CASA) HF_PP_PAR_PARAMETERS_COMMA)

#define HF_PP_VEC_WRAPPERS\
  template <class T> inline HF_PP_GET_FUNCTYPE(T) HF_PP_FUNCNAME(HF_PP_DEF_PARLIST(T,STL)) {HF_PP_VEC_WRAPPER_CODE_STL;} \
  template <class T> inline HF_PP_GET_FUNCTYPE(T) HF_PP_FUNCNAME(HF_PP_DEF_PARLIST(T,CASA)) {HF_PP_VEC_WRAPPER_CODE_CASA;} 

#define HF_PP_VEC_WRAPPERS_hFILE\
  template <class T> inline HF_PP_GET_FUNCTYPE(T) HF_PP_FUNCNAME (HF_PP_DEF_PARLIST(T,STL)); \
  template <class T> inline HF_PP_GET_FUNCTYPE(T) HF_PP_FUNCNAME (HF_PP_DEF_PARLIST(T,CASA)); 

#define HF_PP_GENERATE_WRAPPERS_CC HF_PP_NUM_PYTHON_WRAPPERS HF_PP_VEC_WRAPPERS
#define HF_PP_GENERATE_WRAPPERS_hFILE HF_PP_VEC_WRAPPERS_hFILE HF_PP_MAKE_PYTHON_WRAPPERS_hFILE
#define HF_PP_GENERATE_WRAPPERS_PYTHON HF_PP_PYTHON_EXPOSE

//Choose the correct wrapper appropriate for the current type of file (.cc, .h, python exposer)
//This allows one to generate wrappers of different kind, depending on the file type one is using
#define HF_PP_GENERATE_WRAPPERS_FILE(TYPE) HF_PP_GENERATE_WRAPPERS_##TYPE
#define HF_PP_GENERATE_WRAPPERS BOOST_PP_EXPAND(HF_PP_GENERATE_WRAPPERS_FILE HF_PP_FILETYPE())

//--------------------------------------------------------------------------------
// END hfpp.h
//--------------------------------------------------------------------------------

#endif
