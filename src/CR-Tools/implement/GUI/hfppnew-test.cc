
#
#include "hfppnew.h"

// Tell the preprocessor (for generating wrappers) that this is a c++
// source code file (brackets are crucial)
#undef HFPP_FILETYPE
//-----------------------------------------------------------------------
#define HFPP_FILETYPE CC
//-----------------------------------------------------------------------

/*
To test this file, do:
gcc -E -C -P -D H_DEBUG_CPP -I. -I$LOFARSOFT/release/include $LOFARSOFT/src/CR-Tools/implement/GUI/hfppnew-test.cc | gawk '{gsub(" *\\<_H_NL_\\> *","\n"); print}'
*/

//
//===============================================================================
//
//------EXAMPLE------------------------------------------------------------------
//
/*
Now we will start and example which contains all the wrapper definitions
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(hNegate)(3)("Some documentation")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Great vector 1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HInteger)(vec2)()("Great vector 2")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_2 (HNumber)(val)(0)("Nice scalar")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)  
*/
//
#define HFPP_FUNCDEF  (HFPP_TEMPLATED_TYPE)(hNegate)(3)("Some documentation")(HFPP_PAR_IS_VECTOR)(STL)(HFPP_PASS_AS_REFERENCE)
#define HFPP_PARDEF_0 (HFPP_TEMPLATED_TYPE)(vec1)()("Great vector 1")(HFPP_PAR_IS_VECTOR)(STDIT)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_1 (HInteger)(vec2)()("Great vector 2")(HFPP_PAR_IS_VECTOR)(CASA)(HFPP_PASS_AS_REFERENCE) 
#define HFPP_PARDEF_2 (HNumber)(val)(0)("Nice scalar")(HFPP_PAR_IS_SCALAR)()(HFPP_PASS_AS_VALUE)  
//
//For fixed function type use
//#define HFPP_FUNCDEF (HFPP_PAR_IS_SCALAR,STDIT,void,HFPP_PASS_AS_VALUE,hNegate,3,"Some documentation")

#define HFPP_DONOT_MAKE_WRAPPERS
#include "hfppnew-generatewrappers.h"
#undef HFPP_DONOT_MAKE_WRAPPERS

//...Function Definition........................................................
//Type of the return value of the function
/*HFPP_GET_FUNC_TYPE = */ HFPP_GET_FUNC_TYPE
/*HFPP_GET_FUNC_TYPE_TEMPLATED(STL,TT) = */ HFPP_GET_FUNC_TYPE_TEMPLATED(STL,TT)
//Declaration of the function return value type (may include a pass-by-reference token, i.e. & )
/*HFPP_GET_FUNC_TYPE_DECLARATION= */ HFPP_GET_FUNC_TYPE_DECLARATION
//Same as above, but here one can specify the name of a (potential)
//template parameter (or explicit type) and the container class to be
//used
/*HFPP_GET_FUNC_TYPE_DECLARATION_TEMPLATED(STL,TT)= */ HFPP_GET_FUNC_TYPE_DECLARATION_TEMPLATED(STL,TT)
//Function Name
/*HFPP_GET_FUNC_NAME = */ HFPP_GET_FUNC_NAME
//Number of input/output parameters appearing in function definition
/*HFPP_GET_FUNC_PARNUM = */ HFPP_GET_FUNC_PARNUM
//A short documentation string describing the function.
/*HFPP_GET_FUNC_DOC = */ HFPP_GET_FUNC_DOC
//
//Returns the default template type name for the function, if templated (e.g., T or S)
//otherwise return base type (could be "void")
/* HFPP_GET_FUNC_TEMPLATE_NAME= */ HFPP_GET_FUNC_TEMPLATE_NAME

//...Parameter Definition.......................................................
/* HFPP_GET_PAR_NAME(0) = */ HFPP_GET_PAR_NAME(0) 
/* HFPP_GET_PAR_BASETYPE_VAL(0) = */ HFPP_GET_PAR_BASETYPE_VAL(0) 
/* HFPP_GET_PAR_BASETYPE(0) = */ HFPP_GET_PAR_BASETYPE(0) 
/* HFPP_GET_PAR_DEFVAL(0) = */ HFPP_GET_PAR_DEFVAL(0) 
/* HFPP_GET_PAR_DOC(0) = */ HFPP_GET_PAR_DOC(0) 
/* HFPP_GET_PAR_DIM(0) = */ HFPP_GET_PAR_DIM(0) 
/* HFPP_GET_PAR_BASE_CLASS(0) = */ HFPP_GET_PAR_BASE_CLASS(0) 
/* HFPP_GET_PAR_TEMPLATED(0) = */ HFPP_GET_PAR_TEMPLATED(0) 
/* HFPP_GET_PAR_PASSREFERENCE(0)= */ HFPP_GET_PAR_PASSREFERENCE(0)
//
/* HFPP_GET_PAR_NAME(1) = */ HFPP_GET_PAR_NAME(1) 
/* HFPP_GET_PAR_BASETYPE_VAL(1) = */ HFPP_GET_PAR_BASETYPE_VAL(1) 
/* HFPP_GET_PAR_BASETYPE(1) = */ HFPP_GET_PAR_BASETYPE(1) 
/* HFPP_GET_PAR_DEFVAL(1) = */ HFPP_GET_PAR_DEFVAL(1) 
/* HFPP_GET_PAR_DOC(1) = */ HFPP_GET_PAR_DOC(1) 
/* HFPP_GET_PAR_DIM(1) = */ HFPP_GET_PAR_DIM(1) 
/* HFPP_GET_PAR_TEMPLATED(1) = */ HFPP_GET_PAR_TEMPLATED(1) 
/* HFPP_GET_PAR_PASSREFERENCE(1)= */ HFPP_GET_PAR_PASSREFERENCE(1)
//
/* HFPP_GET_PAR_NAME(2) = */ HFPP_GET_PAR_NAME(2) 
/* HFPP_GET_PAR_BASETYPE_VAL(2) = */ HFPP_GET_PAR_BASETYPE_VAL(2) 
/* HFPP_GET_PAR_BASETYPE(2) = */ HFPP_GET_PAR_BASETYPE(2) 
/* HFPP_GET_PAR_DEFVAL(2) = */ HFPP_GET_PAR_DEFVAL(2) 
/* HFPP_GET_PAR_DOC(2) = */ HFPP_GET_PAR_DOC(2) 
/* HFPP_GET_PAR_DIM(2) = */ HFPP_GET_PAR_DIM(2) 
/* HFPP_GET_PAR_TEMPLATED(2) = */ HFPP_GET_PAR_TEMPLATED(2) 
/* HFPP_GET_PAR_PASSREFERENCE(2)= */ HFPP_GET_PAR_PASSREFERENCE(2)
//------------------------------------------------------------------------------
//
/* HFPP_GET_TEMPLATE_PARAMETER_NAME(1)= */ HFPP_GET_TEMPLATE_PARAMETER_NAME(1)
/* HFPP_GET_TEMPLATE_PARAMETER_NAME(2)= */ HFPP_GET_TEMPLATE_PARAMETER_NAME(2)
/
/* HFPP_GET_FUNC_RETURN= */ HFPP_GET_FUNC_RETURN
//
/* HFPP_GET_PAR_TYPE(0,STL)  = */ HFPP_GET_PAR_TYPE(0,STL)
/* HFPP_GET_PAR_TYPE(1,STL)  = */ HFPP_GET_PAR_TYPE(1,STL)
/* HFPP_GET_PAR_TYPE(2,STL)  = */ HFPP_GET_PAR_TYPE(2,STL)
//
/* HFPP_GET_PAR_TYPE(0,CASA)  = */ HFPP_GET_PAR_TYPE(0,CASA)
/* HFPP_GET_PAR_TYPE(1,CASA)  = */ HFPP_GET_PAR_TYPE(1,CASA)
/* HFPP_GET_PAR_TYPE(2,CASA)  = */ HFPP_GET_PAR_TYPE(2,CASA)
//
/* HFPP_GET_PAR_DECLARATION(0,STL)= */ HFPP_GET_PAR_DECLARATION(0,STL)
/* HFPP_GET_PAR_DECLARATION(1,STL)= */ HFPP_GET_PAR_DECLARATION(1,STL)
/* HFPP_GET_PAR_DECLARATION(2,STL)= */ HFPP_GET_PAR_DECLARATION(2,STL)

//
/* HFPP_GET_PAR_DECLARATION_TEMPLATED_MACRO(ZZZ,0,(STL)(TT))= */ HFPP_GET_PAR_DECLARATION_TEMPLATED_MACRO(ZZZ,0,(STL)(TT))
//
//
/* HFPP_GET_PARLIST_DECLARATION(STL)= */ HFPP_GET_PARLIST_DECLARATION(STL)
/* HFPP_GET_PARLIST_DECLARATION(CASA)= */ HFPP_GET_PARLIST_DECLARATION(CASA)
//
/* HFPP_GET_PARLIST_DECLARATION_TEMPLATED(STL,TT)= */ HFPP_GET_PARLIST_DECLARATION_TEMPLATED(STL,TT)
//
//Now we define the functions that actually are used to generate the
//parameters for calling another function of different container
//classes
//
/* HFPP_GET_PAR_INPUT(0,STL)= */ HFPP_GET_PAR_INPUT(0,STL)
/* HFPP_GET_PAR_INPUT(1,STL)= */ HFPP_GET_PAR_INPUT(1,STL)
/* HFPP_GET_PAR_INPUT(2,STL)= */ HFPP_GET_PAR_INPUT(2,STL)
/* HFPP_GET_PARLIST_INPUT(STL) = */ HFPP_GET_PARLIST_INPUT(STL) 
//
/* HFPP_GET_PAR_INPUT(0,CASA)= */ HFPP_GET_PAR_INPUT(0,CASA)
/* HFPP_GET_PAR_INPUT(1,CASA)= */ HFPP_GET_PAR_INPUT(1,CASA)
/* HFPP_GET_PAR_INPUT(2,CASA)= */ HFPP_GET_PAR_INPUT(2,CASA)
/* HFPP_GET_PARLIST_INPUT(CASA) = */ HFPP_GET_PARLIST_INPUT(CASA) 
//
//
/* HFPP_MAKE_WRAPPERS_VEC_CC(STL): */ 
HFPP_MAKE_WRAPPERS_VEC_CC(STL)
//
/* HFPP_MAKE_WRAPPERS_VEC_CC(CASA): */ 
HFPP_MAKE_WRAPPERS_VEC_CC(CASA)
//
//Generate the defintions of the external variables which hold the
//pointer to the functions to be exposed to Python - to be included in
//the hFILE
//
/* HFPP_MAKE_WRAPPER_PYTHON_hFILE(int) = */ HFPP_MAKE_WRAPPER_PYTHON_hFILE(int) 
//
/* HFPP_MAKE_WRAPPERS_PYTHON_hFILE: */ 
HFPP_MAKE_WRAPPERS_PYTHON_hFILE

//Now generate and assigne the variables which hold the pointer to the
//functions to be exposed to Python - to be included in the .cc
//file. Templated functions will then be instatiated for every type
//specified in HFPP_WRAPPER_TYPES

/* HFPP_MAKE_WRAPPER_PYTHON_CC(int) = */ HFPP_MAKE_WRAPPER_PYTHON_CC(int) 
//
/* HFPP_MAKE_WRAPPERS_PYTHON_CC: */ 
HFPP_MAKE_WRAPPERS_PYTHON_CC

//Make the wrapper to be included in the .hpp file that contains the BOOST PYTHON bindings
/* HFPP_MAKE_WRAPPER_PYTHON_hPYTHON(int)= */ HFPP_MAKE_WRAPPER_PYTHON_hPYTHON(int)
/* HFPP_MAKE_WRAPPERS_PYTHON_hPYTHON: */ 
HFPP_MAKE_WRAPPERS_PYTHON_hPYTHON

//========================================================================
/* HFPP_FILETYPE()= */ HFPP_FILETYPE
//========================================================================
/* HFPP_MAKE_WRAPPERS= */ 
HFPP_MAKE_WRAPPERS
//
#undef HFPP_FILETYPE
#define HFPP_FILETYPE hFILE
//========================================================================
/* HFPP_FILETYPE()= */ HFPP_FILETYPE
//========================================================================
/* HFPP_MAKE_WRAPPERS= */ 
HFPP_MAKE_WRAPPERS
//
#undef HFPP_FILETYPE
#define HFPP_FILETYPE hPYTHON
//========================================================================
/* HFPP_FILETYPE()= */ HFPP_FILETYPE
//========================================================================
/* HFPP_MAKE_WRAPPERS= */ 
HFPP_MAKE_WRAPPERS
//

