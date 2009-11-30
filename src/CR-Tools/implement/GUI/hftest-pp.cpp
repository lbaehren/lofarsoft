/*
Test of boost preprocessor programming
test with

gcc -E -I/Users/falcke/LOFAR/usg/release/include hftest-pp.cpp | gawk '/^#/{next} /^[ ]*$/{next} {print}'


Bug fixes:
/Users/falcke/LOFAR/usg/release/include/boost/preprocessor/seq/first_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory
/Users/falcke/LOFAR/usg/release/include/boost/preprocessor/seq/rest_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory

change to: #include <boost/preprocessor/detail/split.hpp>

*/
//--------------------------------------------------------------------------------
// #include "hfprep.hpp"
// --------------------------------------------------------------------------------
// kept the two files in one for testing. Later the part below until
// "END hfprep.hpp" should be put in a separate file

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/repeat.hpp>


//Defines datatypes to iterate wrappers over
#define HF_PP_NUMERIC_TYPES (HInteger)(HNumber)(HComplex)
#define HF_PP_NON_NUMERIC_TYPES (HPointer)(HString)
#define HF_PP_ALL_TYPES BOOST_PP_CAT(HF_PP_NUMERIC_TYPES,HF_PP_NON_NUMERIC_TYPES)


/*
HF_PP_PythonWrapper_Code: Defines a pointer to the function FUNC with
parameters in HF_PP_PARLIST and template class T. The pointer is assigned
the pointer to the corresponding function and later be given to the
Python function that will call it. Through this assignment the
function will also be instantiated with type T!

We can also create dummy functions with HF_PP_Make_PythonWrappers_Dummy
which creates just empty functions of name FUNC and class type T. This
is used to instantiate certain templates that are needed by the GUI
for formal reasons (i.e. typically HString and HPointer functions).

 */

/*

Here is an example input template to generate wrappers for a function
#define HF_PP_FUNCNAME TEST  //The Name of the function
#define HF_PP_FUNCTYPE void  // Function returns no value
#define HF_PP_NVECS 2 //two input/output vectors

#define HF_PP_PAR0 (freeblocklen,HInteger,128,"Length of a block")
#define HF_PP_PAR1 (blocki,HInteger,128,"Length of a block")
#define HF_PP_PAR2 (blocko,HInteger,128,"Length of a block")
#define HF_PP_NPAR 3 //three parameters in total

#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(vec0.begin(),vec0.end(),vec1.begin(),vec1.end())
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(vec0.cbegin(),vec0.cend(),vec1.begin(),vec1.end())
*/

#define DOT LLL

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

//Create the entire parameter list consisting of the defintions of vectors and the other parameters
//HF_PP_PAR_PARLIST will give the list without the type definition
#define HF_PP_DEF_PARLIST(T,VECTYPE) HF_PP_DEF_VECTORLIST(T,HF_PP_NVECS,VECTYPE) BOOST_PP_COMMA_IF(HF_PP_NPAR) HF_PP_DEF_PARAMETERS
#define HF_PP_PAR_PARLIST(T,VECTYPE) HF_PP_PAR_VECTORLIST(HF_PP_NVECS) BOOST_PP_COMMA_IF(HF_PP_NPAR) HF_PP_PAR_PARAMETERS
/*

HF_PP_DEF_PARLIST(T,STL)
HF_PP_PAR_PARLIST(T,STL)

Will produce:

 vector<T> & vec0 , vector<T> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko
 vec0 , vec1 , freeblocklen , blocki , blocko
*/

//Definitions and initialization of the external variables which hold the pointer to the functions to be exposed to Python
#define HF_PP_PYTHON_WRAPPER_CODE(NIL,FUNC,T) HF_PP_FUNCTYPE (*BOOST_PP_CAT(FUNC,T))(HF_PP_DEF_PARLIST(T,STL)) = &FUNC; 
#define HF_PP_PYTHON_WRAPPER_DUMMY_CODE(R,FUNC,T) HF_PP_FUNCTYPE FUNC (HF_PP_DEF_PARLIST(T,STL)) {}
#define HF_PP_MAKE_PYTHON_WRAPPERS(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_WRAPPER_CODE,FUNC,TYPE_LIST)
#define HF_PP_MAKE_PYTHON_WRAPPERS_DUMMY(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_WRAPPER_DUMMY_CODE,FUNC,TYPE_LIST)

//Generate the defintions of the external variables which hold the pointer to the functions to be exposed to Python - to be included in the hFILE
#define HF_PP_PYTHON_WRAPPER_CODE_hFILE(NIL,FUNC,T) extern HF_PP_FUNCTYPE (*BOOST_PP_CAT(FUNC,T))(HF_PP_DEF_PARLIST(T,STL)); 
#define HF_PP_MAKE_PYTHON_WRAPPERS_hFILE BOOST_PP_SEQ_FOR_EACH(HF_PP_PYTHON_WRAPPER_CODE_hFILE,HF_PP_FUNCNAME,HF_PP_ALL_TYPES)

/*
HF_PP_MAKE_PYTHON_WRAPPERS_hFILE

 gives

extern void (*TESTHInteger)( vector<HInteger> & vec0 , vector<HInteger> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko); extern void (*TESTHNumber)( vector<HNumber> & vec0 , vector<HNumber> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko); extern void (*TESTHComplex)( vector<HComplex> & vec0 , vector<HComplex> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko); extern void (*TESTHPointer)( vector<HPointer> & vec0 , vector<HPointer> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko); extern void (*TESTHString)( vector<HString> & vec0 , vector<HString> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko);
*/


#define HF_PP_NUM_PYTHON_WRAPPERS \
  HF_PP_MAKE_PYTHON_WRAPPERS(HF_PP_FUNCNAME,HF_PP_NUMERIC_TYPES) \
  HF_PP_MAKE_PYTHON_WRAPPERS_DUMMY(HF_PP_FUNCNAME,HF_PP_NON_NUMERIC_TYPES) 
/*
HF_PP_NUM_PYTHON_WRAPPERS

Will provide all the wrappers needed for a python wrapper for numerical vectors

void (*TESTHInteger)( vector<HInteger> & vec0 , vector<HInteger> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko) = &TEST; void (*TESTHNumber)( vector<HNumber> & vec0 , vector<HNumber> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko) = &TEST; void (*TESTHComplex)( vector<HComplex> & vec0 , vector<HComplex> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko) = &TEST; void TEST ( vector<HPointer> & vec0 , vector<HPointer> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko) {} void TEST ( vector<HString> & vec0 , vector<HString> & vec1 , HInteger freeblocklen , HInteger blocki , HInteger blocko) {}

*/

#define HF_PP_VEC_WRAPPERS\
  template <class T> inline HF_PP_FUNCTYPE HF_PP_FUNCNAME(HF_PP_DEF_PARLIST(T,STL)) {HF_PP_VEC_WRAPPER_CODE_STL;} \
  template <class T> inline HF_PP_FUNCTYPE HF_PP_FUNCNAME(HF_PP_DEF_PARLIST(T,CASA)) {HF_PP_VEC_WRAPPER_CODE_CASA;} 

#define HF_PP_VEC_WRAPPERS_hFILE\
  template <class T> inline HF_PP_FUNCTYPE HF_PP_FUNCNAME(HF_PP_DEF_PARLIST(T,STL)); \
  template <class T> inline HF_PP_FUNCTYPE HF_PP_FUNCNAME(HF_PP_DEF_PARLIST(T,CASA)); 

#define HF_PP_GENERATE_WRAPPERS \
HF_PP_NUM_PYTHON_WRAPPERS \
HF_PP_VEC_WRAPPERS

#define HF_PP_GENERATE_WRAPPERS_hFILE HF_PP_VEC_WRAPPERS_hFILE HF_PP_MAKE_PYTHON_WRAPPERS_hFILE

//--------------------------------------------------------------------------------
// END hfprep.hpp
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//Now define actual functions
//--------------------------------------------------------------------------------

#define HF_PP_FUNCNAME TEST  //The Name of the function
#define HF_PP_FUNCTYPE void  // Function returns no value
#define HF_PP_NVECS 2 //two input/output vectors
#define HF_PP_NPAR 3 //three parameters in total
#define HF_PP_PAR0 (freeblocklen,HInteger,128,"Length of a block")
#define HF_PP_PAR1 (blocki,HInteger,128,"Length of a block")
#define HF_PP_PAR2 (blocko,HInteger,128,"Length of a block")

#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(STL),HF_PP_PAR_PARAMETERS)
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(CASA),HF_PP_PAR_PARAMETERS)

HF_PP_GENERATE_WRAPPERS 
------
HF_PP_GENERATE_WRAPPERS_hFILE 
======
#include "hfpp-undef.cc"

#define HF_PP_FUNCNAME FOO  //The Name of the function
#define HF_PP_FUNCTYPE HInteger  // Function returns no value
#define HF_PP_NVECS 1 //two input/output vectors

#define HF_PP_PAR0 (para,HInteger,128,"Parameter A")  //Definition of one input parameter
#define HF_PP_NPAR 1                                  //Number of scalar parameters

#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(STL),HF_PP_PAR_PARAMETERS)
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(CASA),HF_PP_PAR_PARAMETERS)

//-------
HF_PP_GENERATE_WRAPPERS 
-----
HF_PP_GENERATE_WRAPPERS_hFILE 
//--------
#include "hfpp-undef.cc"

