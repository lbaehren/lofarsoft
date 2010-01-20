//Include-file for preprocessor functions used to generate different
//wrappers of the HF library

/*
Test of boost preprocessor programming
test with the following commands

First get the relevant c++ flags:

cd $LOFARSOFT/build/cr; export HFLAGS="`make -n hfget | grep hfanalysis | gawk '/^* /{next}/-I/{match($0," -I"); s=substr($0,RSTART); match(s," -o"); s2=substr(s,0,RSTART); print s2}'`"; cd $GUI

Then call gcc, just executing the preprocessor and leaving comments in place
gcc -E -C -P -D H_DEBUG_CPP $HFLAGS -I. $LOFARSOFT/src/CR-Tools/implement/GUI/hfanalysis.cc | gawk '{gsub("\\<_H_NL_ *\\>","\n"); print}' > tst.cc 

Bug fixes:
$LOFARSOF/release/include/boost/preprocessor/seq/first_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory
$LOFARSOF/release/include/boost/preprocessor/seq/rest_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory

change to: #include <boost/preprocessor/detail/split.hpp>

*/

#ifndef HFPP_H
#define HFPP_H

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/comparison.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/stringize.hpp>

//Defines datatypes to iterate wrappers over
#define HFPP_NUMERIC_TYPES (HInteger)(HNumber)(HComplex)
#define HFPP_STRING_TYPES (HString)
#define HFPP_POINTER_TYPES (HPointer)
#define HFPP_NON_NUMERIC_TYPES HFPP_STRING_TYPES HFPP_POINTER_TYPES 
#define HFPP_ALL_TYPES HFPP_NUMERIC_TYPES HFPP_NON_NUMERIC_TYPES
#define HFPP_ALL_PYTHONTYPES HFPP_NUMERIC_TYPES HFPP_STRING_TYPES

//Used for nicer source code to determine how a parameter is passed and whether its type is templated  
#define HFPP_PASS_AS_REFERENCE 1
#define HFPP_PASS_AS_VALUE 0
#define HFPP_TEMPLATED_TYPE 1
#define HFPP_NON_TEMPLATED_TYPE 0

//Tell the GUI how the return value of the library function is returned 
#define HFPP_GUI_RETURN_SCALAR 0      //Function returns a scalar as return value
#define HFPP_GUI_RETURN_VECTOR_IN_PLACE 1 //Function takes vector as input and returns it modified (pass by reference)
#define HFPP_GUI_RETURN_NEW_VECTOR 2 //Functions takes an input vector and returns a new vectors as second argument


/*

Here is an example input template to generate wrappers for a function
//$FILENAME: HFILE=$SELF.h  // This the outputfile that the awk splitter will write source code to

#define HFPP_FILETYPE CC  // Tell the preprocessor that this is a c++ source code file
//#define HFPP_FILETYPE hFILE  // Tell the preprocessor that this is a header file
//#define HFPP_FILETYPE hPYTHON  // Tell the preprocessor that this is a header file for Python expose

//$WRITE_TO HFILE START:
#define HFPP_FUNCNAME TEST  //The Name of the function
#define HFPP_FUNCBRIEF "Performs a sum over the values in a vector and returns the value" //Provide a short description of the function
#define HFPP_FUNCTYPE void  //Return value type of function
#define HFPP_NVECS 2 //Number of (input/output) vectors
#define HFPP_NPAR 3 //Number of scalar parameters

//To generatae wrappers for the GUI objects, the following information can be provided:
#define HFPP_GUI_LIBRARY Math  // Which library will the function be included in
#define HFPP_GUI_BUFFERED 0  //specify whether a local vector is created in the GUI object upon creation (optional)
#define HFPP_NO_GUI 0  //Optional parameter. If set to 1 no GUI wrapper will be generated.

//Tell the wrapper how the fucntion returns it result (as a single-valued return value, or as a vector)
#define HFPP_GUI_RETURN_POLICY  HFPP_GUI_RETURN_SCALAR  

//The following options are currently implemented
HFPP_GUI_RETURN_SCALAR             //Function returns a scalar as return value
HFPP_GUI_RETURN_VECTOR_IN_PLACE    //Function takes vector as input and returns it modified (pass by reference)
HFPP_GUI_RETURN_NEW_VECTOR         //Functions takes an input vector and returns a new vectors as second argument


To define a parameter the following information is provided as a list of 6 elements:
0) name 
1) type (if templated specify the type of the default value, see below)
2) default value - so far only used to initialize the parameters in the GUI
3) description
4) whether or not the type is actually the templated type of the function 
5) whether to pass by reference or by value (output/input parameter)

#define HFPP_PAR0 (var_a, HInteger, 128, "Length of a block",HFPP_TEMPLATED_TYPE,HFPP_PASS_AS_REFERENCE) //Input Parameter
#define HFPP_PAR1 (var_b, T, 128, "Length of a block",HFPP_TEMPLATED_TYPE,HFPP_PASS_AS_VALUE)
#define HFPP_PAR2 (var_c, HInteger, 128, "Length of a block",HFPP_NON_TEMPLATED_TYPE,HFPP_PASS_AS_VALUE))

//Now define the c++ commands that are executed in order to call the underlying function
#define HFPP_VEC_WRAPPER_CODE_STL return HFPP_FUNCNAME(HFPP_PAR_VECTORITERATORS(STL),HFPP_PAR_PARAMETERS)
#define HFPP_VEC_WRAPPER_CODE_CASA return HFPP_FUNCNAME(HFPP_PAR_VECTORITERATORS(CASA),HFPP_PAR_PARAMETERS)

//if no wrapper code is given then default wrappers are generated by including
#include "hfdefaultwrappercode.h"



//HFPP_VEC_WRAPPER_CODE_STL
//HFPP_VEC_WRAPPER_CODE_CASA

//does something like

//return TEST( vec0.begin(),vec0.end() , vec1.begin(),vec1.end(), var_a , var_b , var_c)
//return TEST( vec0.cbegin(),vec0.cend() , vec1.cbegin(),vec1.cend(), var_a , var_b , var_c)


//$WRITE_TO HFILE: HFPP_GENERATE_WRAPPERS_hFILE 
//Now undefine the definitions above
#include "hfpp-undef.cc"
//$WRITE_TO HFILE END --------------------------------------------------

*/

//This is a simple trick to test whether a specific variable is set to true or not at all (=false)
#define HFPP_IS_SET_TEST0 0
#define HFPP_IS_SET_TEST1 1
#define HFPP_IS_SET_TEST2 1
#define HFPP_IS_SET_TEST3 1
#define HFPP_IS_SET_TEST4 1
#define HFPP_IS_SET_TEST5 1
#define HFPP_IS_SET_TEST6 1
#define HFPP_IS_SET_TEST7 1
#define HFPP_IS_SET_TEST8 1
#define HFPP_IS_SET_TEST9 1
#define HFPP_IS_SET_TEST10 1
#define HFPP_IS_SET_TESTHFPP_FUNCTYPE_T 0
#define HFPP_IS_SET_TESTHFPP_VARIANT 0
#define HFPP_IS_SET_TESTHFPP_GUI_UPDATEABLE 0
#define HFPP_IS_SET_TESTHFPP_GUI_BUFFERED 0
#define HFPP_IS_SET_TESTHFPP_NO_GUI 0
#define HFPP_IS_SET(WAT) BOOST_PP_CAT(HFPP_IS_SET_TEST,WAT)
#define HFPP_IF_SET(WAT) BOOST_PP_IF(HFPP_IS_SET(WAT),WAT,0)
#define HFPP_DO_IF_SET(WAT,DOAA,DOBB) BOOST_PP_IF(HFPP_IF_SET(WAT),DOAA,DOBB)

//generate the name of the Nth vector (e.g. vec0 for N=0) 
#define HFPP_VECTORNAME(N) BOOST_PP_CAT(vec,N)
#define HFPP_VECTORNAME3(NMAX,N,data) HFPP_VECTORNAME(N)
#define HFPP_VECTORITERATOR_STL(NMAX,N,data) BOOST_PP_CAT(vec,N).begin(),BOOST_PP_CAT(vec,N).end()
#define HFPP_VECTORITERATOR_CASA(NMAX,N,data) BOOST_PP_CAT(vec,N).cbegin(),BOOST_PP_CAT(vec,N).cend()

//definition of an STL vector of type T and number N=0,1,2... 
#define HFPP_DEF_VECTOR_STL(NMAX,N,T) vector<T> & HFPP_VECTORNAME3(NMAX,N,T)
#define HFPP_DEF_VECTOR_NOREF_STL(NMAX,N,T) vector<T> HFPP_VECTORNAME3(NMAX,N,T)
#define HFPP_DEF_VECTOR_CASA(NMAX,N,T) casa::Vector<T> & HFPP_VECTORNAME3(NMAX,N,T)

//produce the c++ definitions of N vectors of type T using the container type VECTYPE (e.g., STL or CASA) 
#define HFPP_DEF_VECTORLIST(T,N,VECTYPE) BOOST_PP_ENUM(N, BOOST_PP_CAT(HFPP_DEF_VECTOR_,VECTYPE),T)
#define HFPP_PAR_VECTORLIST(N) BOOST_PP_ENUM(N, HFPP_VECTORNAME3,T)
//HFPP_DEF_VECTORLIST(T,2,STL) will call HFPP_DEF_VECTOR_STL/CASA to give:
//  vector<T> & vec0, vector<T> & vec1

//Generates an argument list containing start and end iterators to the input vectors
#define HFPP_PAR_VECTORITERATORS(VECTYPE) BOOST_PP_ENUM(HFPP_NVECS, BOOST_PP_CAT(HFPP_VECTORITERATOR_,VECTYPE),T)
/*
#define HFPP_NVECS 2
HFPP_PAR_VECTORITERATORS(STL)
HFPP_PAR_VECTORITERATORS(CASA)

will give 

 vec0.begin(),vec0.end() , vec1.begin(),vec1.end()
 vec0.cbegin(),vec0.cend() , vec1.cbegin(),vec1.cend()
*/



//Gives the c++ definition of the Nth (scalar) parameter
#define HFPP_DEF_PARAMETER_N(NMAX,N,T) BOOST_PP_IF(BOOST_PP_TUPLE_ELEM(6,4,BOOST_PP_CAT(HFPP_PAR,N)),T,BOOST_PP_TUPLE_ELEM(6,1,BOOST_PP_CAT(HFPP_PAR,N))) BOOST_PP_IF(BOOST_PP_TUPLE_ELEM(6,5,BOOST_PP_CAT(HFPP_PAR,N)),&,) BOOST_PP_TUPLE_ELEM(6,0,BOOST_PP_CAT(HFPP_PAR,N)) 
//Returns the name of the nth parameter
#define HFPP_PAR_PARAMETER_N(NMAX,N,T) BOOST_PP_TUPLE_ELEM(6,0,BOOST_PP_CAT(HFPP_PAR,N)) 
//This returns the nth parameter but surrounded by a mycast if it is of templated type
#define HFPP_PAR_PARAMETER_N_CAST(NMAX,N,T) BOOST_PP_IF(BOOST_PP_TUPLE_ELEM(6,4,BOOST_PP_CAT(HFPP_PAR,N)),mycast<T>,) (  HFPP_PAR_PARAMETER_N(NMAX,N,T) )

//Gives a list of the c++ definitions of all (i.e. N=NPAR) scalar parameters
#define HFPP_DEF_PARAMETERS(T) BOOST_PP_ENUM(HFPP_NPAR,HFPP_DEF_PARAMETER_N,T)
#define HFPP_PAR_PARAMETERS BOOST_PP_ENUM(HFPP_NPAR,HFPP_PAR_PARAMETER_N,T)
#define HFPP_PAR_PARAMETERS_CAST BOOST_PP_ENUM(HFPP_NPAR,HFPP_PAR_PARAMETER_N_CAST,T)
//Adds a preceding comma to the parameterlist if nonzero so that this can be concatenated with another parameterlist
#define HFPP_PAR_PARAMETERS_COMMA BOOST_PP_COMMA_IF(HFPP_NPAR) HFPP_PAR_PARAMETERS
#define HFPP_PAR_PARAMETERS_CAST_COMMA BOOST_PP_COMMA_IF(HFPP_NPAR) HFPP_PAR_PARAMETERS_CAST

//Create the entire parameter list consisting of the defintions of vectors and the other parameters
//HFPP_PAR_PARLIST will give the list without the type definition
#define HFPP_DEF_PARLIST(T,VECTYPE) HFPP_DEF_VECTORLIST(T,HFPP_NVECS,VECTYPE) BOOST_PP_COMMA_IF(HFPP_NPAR) HFPP_DEF_PARAMETERS(T)
#define HFPP_PAR_PARLIST(T,VECTYPE) HFPP_PAR_VECTORLIST(HFPP_NVECS) HFPP_PAR_PARAMETERS_COMMA
/*

HFPP_DEF_PARLIST(T,STL)
HFPP_PAR_PARLIST(T,STL)

Will produce:

 vector<T> & vec0 , vector<T> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c
 vec0 , vec1 , var_a , var_b , var_c
*/

/*
HFPP_PYTHON_WRAPPER_CODE: Defines a pointer to the function FUNC with
parameters in HFPP_PARLIST and template class T. The pointer is assigned
the pointer to the corresponding function and later be given to the
Python function that will call it. Through this assignment the
function will also be instantiated with type T!

We can also create dummy functions with HFPP_PYTHON_WRAPPER_DUMMY_CODE
which creates just empty functions of name FUNC and class type T. This
is used to instantiate certain templates that are needed by the GUI
for formal reasons (i.e. typically HString and HPointer functions).

 */
//Definitions and initialization of the external variables which hold the pointer to the functions to be exposed to Python
#define HFPP_GET_FUNCTYPE(TYPE) BOOST_PP_IF(HFPP_IS_SET(HFPP_FUNCTYPE_T),TYPE,HFPP_FUNCTYPE)
#define HFPP_PYTHON_POINTER_VARIABLE(FUNC,TYPE) BOOST_PP_CAT(BOOST_PP_CAT(FUNC,TYPE),BOOST_PP_IF(HFPP_IS_SET(HFPP_VARIANT),HFPP_VARIANT,))
#define HFPP_PYTHON_WRAPPER_CODE(NIL,FUNC,TYPE) HFPP_GET_FUNCTYPE(TYPE) (*HFPP_PYTHON_POINTER_VARIABLE(FUNC,TYPE))(HFPP_DEF_PARLIST(TYPE,STL)) = &FUNC; 
#define HFPP_PYTHON_WRAPPER_DUMMY_CODE(NIL,FUNC,TYPE) HFPP_GET_FUNCTYPE(TYPE) FUNC (HFPP_DEF_PARLIST(TYPE,STL)) {}
#define HFPP_MAKE_PYTHON_WRAPPERS(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HFPP_PYTHON_WRAPPER_CODE,FUNC,TYPE_LIST)
#define HFPP_MAKE_PYTHON_WRAPPERS_DUMMY(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HFPP_PYTHON_WRAPPER_DUMMY_CODE,FUNC,TYPE_LIST)

#define HFPP_PYTHON_EXPOSE_CODE(NIL,FUNC,TYPE) def(#FUNC,HFPP_PYTHON_POINTER_VARIABLE(FUNC,TYPE));
#define HFPP_PYTHON_EXPOSE BOOST_PP_SEQ_FOR_EACH(HFPP_PYTHON_EXPOSE_CODE,HFPP_FUNCNAME,HFPP_ALL_PYTHONTYPES)

//Generate the defintions of the external variables which hold the pointer to the functions to be exposed to Python - to be included in the hFILE
#define HFPP_PYTHON_WRAPPER_CODE_hFILE(NIL,FUNC,T) extern HFPP_GET_FUNCTYPE(T) (*HFPP_PYTHON_POINTER_VARIABLE(FUNC,T))(HFPP_DEF_PARLIST(T,STL)); 
#define HFPP_MAKE_PYTHON_WRAPPERS_hFILE BOOST_PP_SEQ_FOR_EACH(HFPP_PYTHON_WRAPPER_CODE_hFILE,HFPP_FUNCNAME,HFPP_ALL_TYPES)

/*
HFPP_MAKE_PYTHON_WRAPPERS_hFILE

 gives

extern void (*TESTHInteger)( vector<HInteger> & vec0 , vector<HInteger> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHNumber)( vector<HNumber> & vec0 , vector<HNumber> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHComplex)( vector<HComplex> & vec0 , vector<HComplex> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHPointer)( vector<HPointer> & vec0 , vector<HPointer> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c); extern void (*TESTHString)( vector<HString> & vec0 , vector<HString> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c);
*/


#define HFPP_NUM_PYTHON_WRAPPERS \
  HFPP_MAKE_PYTHON_WRAPPERS_DUMMY(HFPP_FUNCNAME,HFPP_NON_NUMERIC_TYPES) \
  HFPP_MAKE_PYTHON_WRAPPERS(HFPP_FUNCNAME,HFPP_ALL_TYPES)

/*
HFPP_NUM_PYTHON_WRAPPERS

Will provide all the wrappers needed for a python wrapper for numerical vectors

void (*TESTHInteger)( vector<HInteger> & vec0 , vector<HInteger> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) = &TEST; void (*TESTHNumber)( vector<HNumber> & vec0 , vector<HNumber> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) = &TEST; void (*TESTHComplex)( vector<HComplex> & vec0 , vector<HComplex> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) = &TEST; void TEST ( vector<HPointer> & vec0 , vector<HPointer> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) {} void TEST ( vector<HString> & vec0 , vector<HString> & vec1 , HInteger var_a , HInteger var_b , HInteger var_c) {}

*/

#define HFPP_VEC_WRAPPER_CODE_STL_DEFAULT(FUNC) return FUNC(HFPP_PAR_VECTORITERATORS(STL) HFPP_PAR_PARAMETERS_COMMA)
#define HFPP_VEC_WRAPPER_CODE_CASA_DEFAULT(FUNC) return FUNC(HFPP_PAR_VECTORITERATORS(CASA) HFPP_PAR_PARAMETERS_COMMA)

#define HFPP_VEC_WRAPPER_CODE_GUI_DEFAULTHFPP_GUI_RETURN_POLICY HFPP_VEC_WRAPPER_CODE_GUI_DEFAULT1 //Sets a default in case Return Policy is not defined.
#define HFPP_VEC_WRAPPER_CODE_GUI_DEFAULT0(FUNC) dp->getFirstFromVector(*vp,vs); copyvec(mycast<T>(FUNC(*vp HFPP_PAR_PARAMETERS_CAST_COMMA)),*vp)
#define HFPP_VEC_WRAPPER_CODE_GUI_DEFAULT1(FUNC) dp->getFirstFromVector(*vp,vs); FUNC(*vp HFPP_PAR_PARAMETERS_CAST_COMMA)
#define HFPP_VEC_WRAPPER_CODE_GUI_DEFAULT2(FUNC) dp->getFirstFromVector(*vp,vs); HFPP_DEF_VECTOR_NOREF_STL(1,1,T); FUNC(*vp,HFPP_VECTORNAME(1) HFPP_PAR_PARAMETERS_CAST_COMMA);  copyvec(HFPP_VECTORNAME(1),*vp)
#define HFPP_VEC_WRAPPER_CODE_GUI_DEFAULT(FUNC) BOOST_PP_CAT(HFPP_VEC_WRAPPER_CODE_GUI_DEFAULT,HFPP_GUI_RETURN_POLICY)(FUNC)

#define HFPP_VEC_WRAPPERS\
  template <class T> inline HFPP_GET_FUNCTYPE(T) HFPP_FUNCNAME(HFPP_DEF_PARLIST(T,STL)) {HFPP_VEC_WRAPPER_CODE_STL;} \
  template <class T> inline HFPP_GET_FUNCTYPE(T) HFPP_FUNCNAME(HFPP_DEF_PARLIST(T,CASA)) {HFPP_VEC_WRAPPER_CODE_CASA;} 

#define HFPP_VEC_WRAPPERS_hFILE\
  template <class T> inline HFPP_GET_FUNCTYPE(T) HFPP_FUNCNAME (HFPP_DEF_PARLIST(T,STL)); \
  template <class T> inline HFPP_GET_FUNCTYPE(T) HFPP_FUNCNAME (HFPP_DEF_PARLIST(T,CASA)); 

//------------------------------------------------------------------------------
//define wrappers for the GUI
//------------------------------------------------------------------------------

//will give something like DataFunc_$Lib_$Name
#define HFPP_GUI_MAKE_FUNCNAME BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(DataFunc_,HFPP_GUI_LIBRARY),_),HFPP_FUNCNAME)

//Make list for one parameter
#define HFPP_GUI_PARAMETER_N(N) BOOST_PP_TUPLE_ELEM(6,0,BOOST_PP_CAT(HFPP_PAR,N)),BOOST_PP_TUPLE_ELEM(6,1,BOOST_PP_CAT(HFPP_PAR,N)),BOOST_PP_TUPLE_ELEM(6,2,BOOST_PP_CAT(HFPP_PAR,N))
#define HFPP_GUI_VECPARAMETER_N(N) BOOST_PP_TUPLE_ELEM(6,0,BOOST_PP_CAT(HFPP_PAR,N)),BOOST_PP_TUPLE_ELEM(6,1,BOOST_PP_CAT(HFPP_PAR,N))


#define HFPP_GUI_SETPARAMETERS(MMM,N,DDD) BOOST_PP_CAT(SET_FUNC_PARAMETER,_AWK(HFPP_GUI_PARAMETER_N(N)))
#define HFPP_GUI_SETVECPARAMETERS(MMM,N,DDD) BOOST_PP_CAT(SET_FUNC_VECPARAMETER,_AWK(HFPP_GUI_PARAMETER_N(N)))
#define HFPP_GUI_GETPARAMETERS(MMM,N,DDD) BOOST_PP_CAT(GET_FUNC_PARAMETER,_AWK(HFPP_GUI_PARAMETER_N(N)))
#define HFPP_GUI_GETVECPARAMETERS(MMM,N,DDD) BOOST_PP_CAT(GET_FUNC_VECPARAMETER,_AWK(HFPP_GUI_PARAMETER_N(N)))


#define HFPP_GUI_GETTYPEvoid NUMBER /// This is just a dirty fix ....
#define HFPP_GUI_GETTYPEHFPP_FUNCTYPE NUMBER /// This is just a dirty fix ....
#define HFPP_GUI_GETTYPEHPointer POINTER
#define HFPP_GUI_GETTYPEHString STRING
#define HFPP_GUI_GETTYPEHInteger INTEGER
#define HFPP_GUI_GETTYPEHNumber NUMBER
#define HFPP_GUI_GETTYPEHComplex COMPLEX
#define HFPP_GUI_GETTYPE(TYPE) BOOST_PP_CAT(HFPP_GUI_GETTYPE,TYPE)


//Note that DEFINE_PROCESS_CALLS_NUMONLY needs to be changed to
//DEFINE_PROCESS_CALLS in case there are functions which operate on 
//pointer or string vectors

#define HFPP_GUI_WRAPPERS\
  class  HFPP_GUI_MAKE_FUNCNAME : public ObjectFunctionClass { \
public: \
 DEFINE_PROCESS_CALLS_NUMONLY\
 HFPP_GUI_MAKE_FUNCNAME (Data* dp) : ObjectFunctionClass(dp){	\
   dp->setUpdateable(HFPP_IF_SET(HFPP_GUI_UPDATEABLE));	\
    setParameters();\
    startup();\
    getParameters();\
    }\
 ~HFPP_GUI_MAKE_FUNCNAME (){cleanup(); } \
void setParameters(){\
  BOOST_PP_REPEAT(HFPP_NPAR, HFPP_GUI_SETPARAMETERS,DDD) BOOST_PP_IF(BOOST_PP_GREATER_EQUAL(HFPP_NPAR,1),;,) \
};\
template <class T> void process(F_PARAMETERS) {\
  BOOST_PP_IF(BOOST_PP_GREATER_EQUAL(HFPP_NPAR,1),BOOST_PP_REPEAT(HFPP_NPAR, HFPP_GUI_GETPARAMETERS,DDD);,) \
  HFPP_VEC_WRAPPER_CODE_GUI ;\
}}; DATAFUNC_CONSTRUCTOR(HFPP_FUNCNAME,HFPP_GUI_LIBRARY,HFPP_FUNCBRIEF,UNDEF,HFPP_IF_SET(HFPP_GUI_BUFFERED));

//This is needed to add the functions to the GUI function library
#define HFPP_GUI_WRAPPERS_PUBLISH PUBLISH_OBJECT_FUNCTION(HFPP_GUI_LIBRARY,HFPP_FUNCNAME); 


#define HFPP_GENERATE_WRAPPERS_CC HFPP_NUM_PYTHON_WRAPPERS HFPP_VEC_WRAPPERS
#define HFPP_GENERATE_WRAPPERS_hFILE HFPP_VEC_WRAPPERS_hFILE HFPP_MAKE_PYTHON_WRAPPERS_hFILE
#define HFPP_GENERATE_WRAPPERS_PYTHON HFPP_PYTHON_EXPOSE
#define HFPP_GENERATE_WRAPPERS_GUI HFPP_DO_IF_SET(HFPP_NO_GUI, ,HFPP_GUI_WRAPPERS)
#define HFPP_GENERATE_WRAPPERS_GUIPUBLISH HFPP_DO_IF_SET(HFPP_NO_GUI,,HFPP_GUI_WRAPPERS_PUBLISH)

//Choose the correct wrapper appropriate for the current type of file (.cc, .h, python exposer)
//This allows one to generate wrappers of different kind, depending on the file type one is using
#define HFPP_GENERATE_WRAPPERS_FILE(TYPE) HFPP_GENERATE_WRAPPERS_##TYPE
#define HFPP_GENERATE_WRAPPERS BOOST_PP_EXPAND(HFPP_GENERATE_WRAPPERS_FILE HFPP_FILETYPE())

//--------------------------------------------------------------------------------
// END hfpp.h
//--------------------------------------------------------------------------------

#endif
