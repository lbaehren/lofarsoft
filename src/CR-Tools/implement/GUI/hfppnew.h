/*
First get the relevant c++ flags:

cd $LOFARSOFT/build/cr; export HFLAGS="`make -n hfget | grep hfanalysis | gawk '/^* /{next}/-I/{match($0," -I"); s=substr($0,RSTART); match(s," -o"); s2=substr(s,0,RSTART); print s2}'`"; cd $GUI

Then call gcc, just executing the preprocessor and leaving comments in place
gcc -E -C -P -D H_DEBUG_CPP $HFLAGS -I. $LOFARSOFT/src/CR-Tools/implement/GUI/hfppnew-test.cc | gawk '{gsub(" *\\<_H_NL_ *\\>","\n"); print}' > tst.cc

gcc -E -C -P -D H_DEBUG_CPP -I. $LOFARSOFT/src/CR-Tools/implement/GUI/hfppnew-test.cc | gawk '{gsub(" *\\< _H_NL_\\ *>","\n"); print}' > tst.cc

gcc -E -C -P -D H_DEBUG_CPP $HFLAGS -I. $LOFARSOFT/src/CR-Tools/implement/GUI/hfanalysis.cc | gawk '{gsub(" *\\<_H_NL_\\ *>","\n"); print}' > tst.cc 

gcc -E -C -P -D H_DEBUG_CPP $HFLAGS -I. $LOFARSOFT/src/CR-Tools/implement/GUI/hfppnew-test.cc | gawk '{gsub(" *\\<_H_NL_\\> *","\n"); print}'
*/


#ifndef HFPP_H
#define HFPP_H

//This allows one to generate an output where newlines are inserted by another preprocessor
#ifndef H_DEBUG_CPP
#define _H_NL_ 
#endif

#define HFPP_VERBOSE 1 

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/comparison.hpp>
#include <boost/preprocessor/facilities/expand.hpp>
#include <boost/preprocessor/stringize.hpp>

//Some convenience definitions
#define HFPP_TRUE 1
#define HFPP_FALSE 0

//Defines datatypes to iterate wrappers over
#define HFPP_NUMERIC_TYPES (HInteger)(HNumber)(HComplex)
#define HFPP_STRING_TYPES (HString)
#define HFPP_POINTER_TYPES (HPointer)
#define HFPP_NON_NUMERIC_TYPES HFPP_STRING_TYPES HFPP_POINTER_TYPES 
#define HFPP_ALL_TYPES HFPP_NUMERIC_TYPES HFPP_NON_NUMERIC_TYPES
#define HFPP_ALL_PYTHONTYPES HFPP_NUMERIC_TYPES HFPP_STRING_TYPES

#define HFPP_DEFAULT_TEMPLATE_PARAMETER_NAMES ()(T)(S)
#define HFPP_DEFAULT_WRAPPER_LIST (STL)(CASA)
#define HFPP_DEFAULT_WRAPPER_TYPES HFPP_NUMERIC_TYPES

//Used for nicer source code to determine how a parameter is passed and whether its type is templated  
#define HFPP_PASS_AS_REFERENCE 1
#define HFPP_PASS_AS_VALUE 0
#define HFPP_TEMPLATED_TYPE 1
#define HFPP_NON_TEMPLATED_TYPE 0

#define HFPP_PAR_IS_SCALAR 0
#define HFPP_PAR_IS_VECTOR 1
#define HFPP_PAR_IS_MATRIX 2

//Tell the GUI how the return value of the library function is returned 
#define HFPP_GUI_RETURN_SCALAR 0      //Function returns a scalar as return value
#define HFPP_GUI_RETURN_VECTOR_IN_PLACE 1 //Function takes vector as input and returns it modified (pass by reference)
#define HFPP_GUI_RETURN_NEW_VECTOR 2 //Functions takes an input vector and returns a new vectors as second argument


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



//------------------------------------------------------------------------
//Function Defintion
//------------------------------------------------------------------------
#define HFPP_LEN_FUNCDEF 7
#define HFPP_LEN_PARDEF 8

#define HFPP_GET_FUNC_NAME BOOST_PP_SEQ_ELEM(0,HFPP_FUNCDEF)
#define HFPP_GET_FUNC_PARNUM BOOST_PP_SEQ_ELEM(1,HFPP_FUNCDEF)
#define HFPP_GET_FUNC_DOC BOOST_PP_SEQ_ELEM(2,HFPP_FUNCDEF)
#define HFPP_GET_FUNC_DIM BOOST_PP_SEQ_ELEM(3,HFPP_FUNCDEF)
#define HFPP_GET_FUNC_BASE_CLASS BOOST_PP_SEQ_ELEM(4,HFPP_FUNCDEF)
#define HFPP_GET_FUNC_TYPE_VALUE BOOST_PP_SEQ_ELEM(5,HFPP_FUNCDEF)
#define HFPP_GET_FUNC_PASSREFERENCE BOOST_PP_SEQ_ELEM(6,HFPP_FUNCDEF)

#define HFPP_GET_PAR_NAME(N) BOOST_PP_SEQ_ELEM(0,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_BASETYPE(N)  BOOST_PP_SEQ_ELEM(1,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_DEFVAL(N) BOOST_PP_SEQ_ELEM(2,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_DOC(N) BOOST_PP_SEQ_ELEM(3,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_DIM(N) BOOST_PP_SEQ_ELEM(4,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_BASE_CLASS(N) BOOST_PP_SEQ_ELEM(5,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_TEMPLATED(N) BOOST_PP_SEQ_ELEM(6,BOOST_PP_CAT(HFPP_PARDEF_,N))
#define HFPP_GET_PAR_PASSREFERENCE(N) BOOST_PP_SEQ_ELEM(7,BOOST_PP_CAT(HFPP_PARDEF_,N))

#define HFPP_GET_TEMPLATE_PARAMETER_NAME(N) BOOST_PP_IF(BOOST_PP_EQUAL(N,0),BOOST_PP_EMPTY(),BOOST_PP_SEQ_ELEM(N,HFPP_TEMPLATE_PARAMETER_NAMES))

//Definitions to make string comparisons
#define HFPP_IS_EQUALvoid 101

//------------------------------------------------------------------------------
//Define parameter declarations for different dimensions and different containers
//------------------------------------------------------------------------------
#define HFPP_GET_PAR_DECLARATION_DIM0STL(TYPE) TYPE
#define HFPP_GET_PAR_DECLARATION_DIM1STL(TYPE) std::vector<TYPE>

#define HFPP_GET_PAR_DECLARATION_DIM0CASA(TYPE) TYPE
#define HFPP_GET_PAR_DECLARATION_DIM1CASA(TYPE) casa::Vector<TYPE>
//------------------------------------------------------------------------------

//Returns the default template type name for the function, if templated (e.g., T or S)
//otherwise return base type (could be "void")
#define HFPP_GET_FUNC_TEMPLATE_NAME BOOST_PP_IF(HFPP_FUNC_TYPE_IS_TEMPLATED,HFPP_GET_TEMPLATE_PARAMETER_NAME(HFPP_GET_FUNC_TYPE_VALUE),HFPP_GET_FUNC_BASETYPE)

//Returns the default function return type
#define HFPP_GET_FUNC_TYPE_TEMPLATED(BASECLASS,TEMPLATETYPE) BOOST_PP_CAT(BOOST_PP_CAT(HFPP_GET_PAR_DECLARATION_DIM,HFPP_GET_FUNC_DIM),BOOST_PP_IF(HFPP_FUNC_BASE_CLASS_ALLOWED_TO_CHANGE,BASECLASS,HFPP_GET_FUNC_BASE_CLASS))(BOOST_PP_IF(HFPP_FUNC_TYPE_IS_TEMPLATED,TEMPLATETYPE,HFPP_GET_FUNC_BASETYPE))  

#define HFPP_GET_FUNC_TYPE HFPP_GET_FUNC_TYPE_TEMPLATED(HFPP_GET_FUNC_BASE_CLASS,HFPP_GET_FUNC_TEMPLATE_NAME)

#define HFPP_GET_FUNC_TYPE_DECLARATION_TEMPLATED(BASECLASS,TEMPLATETYPE) HFPP_GET_FUNC_TYPE_TEMPLATED(BASECLASS,TEMPLATETYPE) BOOST_PP_IF(HFPP_GET_FUNC_PASSREFERENCE,&,BOOST_PP_EMPTY())
#define HFPP_GET_FUNC_TYPE_DECLARATION(BASECLASS,TEMPLATETYPE) HFPP_GET_FUNC_TYPE_DECLARATION_TEMPLATED(HFPP_GET_FUNC_BASE_CLASS,HFPP_GET_FUNC_TEMPLATE_NAME)


//Generates the full type of a parameter, i.e. including the container type, like "vector<HInteger>"
#define HFPP_GET_PAR_TYPE_TEMPLATED(N,WRAPPERTYPE,TEMPLATETYPE) BOOST_PP_CAT(BOOST_PP_CAT(HFPP_GET_PAR_DECLARATION_DIM,HFPP_GET_PAR_DIM(N)),WRAPPERTYPE)(BOOST_PP_IF(HFPP_GET_PAR_TEMPLATED(N),TEMPLATETYPE,HFPP_GET_PAR_BASETYPE(N)))  
#define HFPP_GET_PAR_TYPE(N,WRAPPERTYPE) HFPP_GET_PAR_TYPE_TEMPLATED(N,WRAPPERTYPE,HFPP_GET_TEMPLATE_PARAMETER_NAME(HFPP_GET_PAR_TEMPLATED(N)))

//Generate a parameter declaration, that can go into the parameter list of the function definition: "vector<HInteger>& vec"
#define HFPP_GET_PAR_DECLARATION_TEMPLATED(N,WRAPPERTYPE,TEMPLATETYPE) HFPP_GET_PAR_TYPE_TEMPLATED(N,WRAPPERTYPE,TEMPLATETYPE) BOOST_PP_IF(HFPP_GET_PAR_PASSREFERENCE(N),&,BOOST_PP_EMPTY()) HFPP_GET_PAR_NAME(N)
#define HFPP_GET_PAR_DECLARATION(N,WRAPPERTYPE)  HFPP_GET_PAR_DECLARATION_TEMPLATED(N,WRAPPERTYPE,HFPP_GET_TEMPLATE_PARAMETER_NAME(HFPP_GET_PAR_TEMPLATED(N)))

#define HFPP_GET_PAR_DECLARATION_TEMPLATED_MACRO(ZZZ,N,WRAPPERTYPE_AND_TEMPLATETYPE_LIST) HFPP_GET_PAR_DECLARATION_TEMPLATED(N,BOOST_PP_SEQ_ELEM(0,WRAPPERTYPE_AND_TEMPLATETYPE_LIST),BOOST_PP_SEQ_ELEM(1,WRAPPERTYPE_AND_TEMPLATETYPE_LIST))
#define HFPP_GET_PAR_DECLARATION_MACRO(ZZZ,N,WRAPPERTYPE) HFPP_GET_PAR_DECLARATION(N,WRAPPERTYPE)

//Generate the entire parameterlist that is in the function definition: "std::vector<T> & vec , std::vector<HInteger> & vec , HInteger val"
#define HFPP_GET_PARLIST_DECLARATION(WRAPPERTYPE) BOOST_PP_ENUM(HFPP_GET_FUNC_PARNUM,HFPP_GET_PAR_DECLARATION_MACRO,WRAPPERTYPE)
#define HFPP_GET_PARLIST_DECLARATION_TEMPLATED(WRAPPERTYPE,TEMPLATETYPE) BOOST_PP_ENUM(HFPP_GET_FUNC_PARNUM,HFPP_GET_PAR_DECLARATION_TEMPLATED_MACRO,(WRAPPERTYPE)(TEMPLATETYPE))


//Now we define the functions that actually are used to generate the parameters for calling another function of different container classes 
#define HFPP_CONVERT_PAR_INPUT_STL_TO_0(SCALAR) SCALAR
#define HFPP_CONVERT_PAR_INPUT_CASA_TO_0(SCALAR) SCALAR
#define HFPP_CONVERT_PAR_INPUT_STDIT_TO_0(SCALAR) SCALAR

#define HFPP_CONVERT_PAR_INPUT_STL_TO_STDIT0(SCALAR) SCALAR
#define HFPP_CONVERT_PAR_INPUT_STL_TO_STDIT1(VEC) VEC.begin(),VEC.end()
#define HFPP_CONVERT_PAR_INPUT_STL_TO_CASA1(VEC) stl2casa(VEC)
#define HFPP_CONVERT_PAR_INPUT_STL_TO_STL1(VEC) VEC

#define HFPP_CONVERT_PAR_INPUT_CASA_TO_STDIT0(SCALAR) SCALAR
#define HFPP_CONVERT_PAR_INPUT_CASA_TO_STDIT1(VEC) VEC.cbegin(),VEC.cend()
#define HFPP_CONVERT_PAR_INPUT_CASA_TO_CASA1(VEC) VEC
#define HFPP_CONVERT_PAR_INPUT_CASA_TO_STL1(VEC) casa2stl(VEC)

//Calls the above defined converter functions
#define HFPP_CONVERT_PAR_INPUT(DIM,FROM_WRAPPERTYPE,TO_WRAPPERTYPE,VEC) BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(BOOST_PP_CAT(HFPP_CONVERT_PAR_INPUT_,FROM_WRAPPERTYPE),_TO_),TO_WRAPPERTYPE),DIM)(VEC)

//delivers a form suitable to be used as input parameters for calling a function using a different container class (e.g., VEC -> VEC.begin(),VEC.end())
#define HFPP_GET_PAR_INPUT(N,FROM_WRAPPERTYPE)  HFPP_CONVERT_PAR_INPUT(HFPP_GET_PAR_DIM(N),FROM_WRAPPERTYPE,HFPP_GET_PAR_BASE_CLASS(N),HFPP_GET_PAR_NAME(N))
#define HFPP_GET_PAR_INPUT_MACRO(ZZZ,N,FROM_WRAPPERTYPE)  HFPP_GET_PAR_INPUT(N,FROM_WRAPPERTYPE)

#define HFPP_GET_PARLIST_INPUT(WRAPPERTYPE) BOOST_PP_ENUM(HFPP_GET_FUNC_PARNUM,HFPP_GET_PAR_INPUT_MACRO,WRAPPERTYPE)

//Now generate the Wrappers of certain types and for certain file type 
//wrappers for stl vectors in the .cc (actual source code) file
#define HFPP_MAKE_WRAPPERS_VEC_CC(WRAPPERTYPE) \
  template <class T> inline HFPP_GET_FUNC_BASETYPE HFPP_GET_FUNC_NAME(HFPP_GET_PARLIST_DECLARATION(STL)) { _H_NL_\
      HFPP_GET_FUNC_RETURN HFPP_GET_FUNC_NAME  (HFPP_GET_PARLIST_INPUT(WRAPPERTYPE)); _H_NL_ \
  } 

#define HFPP_MAKE_WRAPPERS_VEC_hFILE(WRAPPERTYPE)\
  template <class T> inline HFPP_GET_FUNC_BASETYPE HFPP_GET_FUNC_NAME(HFPP_GET_PARLIST_DECLARATION(WRAPPERTYPE));


#define HFPP_GET_PYTHON_POINTER_VARIABLE(FUNC,TYPE) BOOST_PP_CAT(BOOST_PP_CAT(FUNC,TYPE),BOOST_PP_IF(HFPP_IS_SET(HFPP_VARIANT),HFPP_VARIANT,BOOST_PP_EMPTY()))

//Generate the defintions of the external variables which hold the
//pointer to the functions to be exposed to Python - to be included in
//the hFILE
#define HFPP_MAKE_WRAPPER_MACRO_PYTHON_hFILE(ZZZ,YYY,TYPE) HFPP_MAKE_WRAPPER_PYTHON_hFILE(TYPE) _H_NL_
#define HFPP_MAKE_WRAPPERS_PYTHON_hFILE BOOST_PP_SEQ_FOR_EACH(HFPP_MAKE_WRAPPER_MACRO_PYTHON_hFILE,BOOST_PP_EMPTY(),HFPP_WRAPPER_TYPES)

#define HFPP_MAKE_WRAPPER_PYTHON_hFILE(TYPE) \
  extern HFPP_GET_FUNC_TYPE_DECLARATION_TEMPLATED(STL,TYPE) (*HFPP_GET_PYTHON_POINTER_VARIABLE(HFPP_GET_FUNC_NAME,TYPE))(HFPP_GET_PARLIST_DECLARATION_TEMPLATED(STL,TYPE)); 

//Generate the variables which hold the pointer to the functions to be
//exposed to Python - to be included in the .cc file
#define HFPP_MAKE_WRAPPER_MACRO_PYTHON_CC(ZZZ,YYY,TYPE) HFPP_MAKE_WRAPPER_PYTHON_CC(TYPE) _H_NL_
#define HFPP_MAKE_WRAPPERS_PYTHON_CC BOOST_PP_SEQ_FOR_EACH(HFPP_MAKE_WRAPPER_MACRO_PYTHON_CC,BOOST_PP_EMPTY(),HFPP_WRAPPER_TYPES)

#define HFPP_MAKE_WRAPPER_PYTHON_CC(TYPE) \
  HFPP_GET_FUNC_TYPE_DECLARATION_TEMPLATED(STL,TYPE) (*HFPP_GET_PYTHON_POINTER_VARIABLE(HFPP_GET_FUNC_NAME,TYPE))(HFPP_GET_PARLIST_DECLARATION_TEMPLATED(STL,TYPE)) = &HFPP_GET_FUNC_NAME; 

//Make the wrapper to be included in the .hpp file that contains the BOOST PYTHON bindings
#define HFPP_MAKE_WRAPPER_MACRO_PYTHON_hPYTHON(ZZZ,YYY,TYPE) HFPP_MAKE_WRAPPER_PYTHON_hPYTHON(TYPE) _H_NL_
#define HFPP_MAKE_WRAPPERS_PYTHON_hPYTHON BOOST_PP_SEQ_FOR_EACH(HFPP_MAKE_WRAPPER_MACRO_PYTHON_hPYTHON,BOOST_PP_EMPTY(),HFPP_WRAPPER_TYPES)

#define HFPP_MAKE_WRAPPER_PYTHON_hPYTHON(TYPE) \
  def(BOOST_PP_STRINGIZE(HFPP_GET_FUNC_NAME),HFPP_GET_PYTHON_POINTER_VARIABLE(HFPP_GET_FUNC_NAME,TYPE));


#endif
