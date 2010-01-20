//
//GENERATING WRAPPERS
//


//First set all the values to default values, if not present
//------------------------------------------------------------------------------

#ifndef HFPP_FUNC_BASE_CLASS_ALLOWED_TO_CHANGE
#define HFPP_FUNC_BASE_CLASS_ALLOWED_TO_CHANGE HFPP_TRUE
#else
#define HFPP_FUNC_BASE_CLASS_ALLOWED_TO_CHANGE HFPP_FALSE
#endif

#ifndef HFPP_WRAPPER_CLASSES
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
#define HFPP_WRAPPER_CLASSES HFPP_DEFAULT_WRAPPER_CLASSES
#endif

#ifndef HFPP_MAKE_PYTHONWRAPPERS
#define  HFPP_MAKE_PYTHONWRAPPERS HFPP_TRUE
#endif

//Defines all the different base types (int, double, ...) that will be
//instantiated for template parameters. Important e.g. for pythn
//bindings
#ifndef HFPP_WRAPPER_TYPES
#define HFPP_WRAPPER_TYPES HFPP_DEFAULT_WRAPPER_TYPES
#endif

//Defines names of the template parameters (e.g., T, S) that are being
//used in templated function defintions (e.g., template <class T>)
#ifndef HFPP_TEMPLATE_PARAMETER_NAMES 
#define HFPP_TEMPLATE_PARAMETER_NAMES HFPP_DEFAULT_TEMPLATE_PARAMETER_NAMES
#endif

//Now set some definitions based on conditionals
//------------------------------------------------------------------------------
#if HFPP_GET_FUNC_TYPE_VALUE > 0
#define HFPP_FUNC_TYPE_IS_TEMPLATED HFPP_GET_FUNC_TYPE_VALUE
#define HFPP_GET_FUNC_BASETYPE BOOST_PP_SEQ_ELEM(HFPP_GET_FUNC_TYPE_VALUE,HFPP_TEMPLATE_PARAMETER_NAMES) 
#else
#define HFPP_FUNC_TYPE_IS_TEMPLATED HFPP_FALSE
#define HFPP_GET_FUNC_BASETYPE HFPP_GET_FUNC_TYPE_VALUE
#endif

#if HFPP_GET_FUNC_PARNUM > 0
#if HFPP_GET_PAR_BASETYPE_VAL(0) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_0 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_0  HFPP_GET_PAR_BASETYPE_VAL(0)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_0 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_0  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 1
#if HFPP_GET_PAR_BASETYPE_VAL(1) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_1 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_1  HFPP_GET_PAR_BASETYPE_VAL(1)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_1 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_1  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 2
#if HFPP_GET_PAR_BASETYPE_VAL(2) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_2 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_2  HFPP_GET_PAR_BASETYPE_VAL(2)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_2 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_2  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 3
#if HFPP_GET_PAR_BASETYPE_VAL(3) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_3 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_3  HFPP_GET_PAR_BASETYPE_VAL(3)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_3 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_3  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 4
#if HFPP_GET_PAR_BASETYPE_VAL(4) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_4 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_4  HFPP_GET_PAR_BASETYPE_VAL(4)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_4 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_4  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 5
#if HFPP_GET_PAR_BASETYPE_VAL(5) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_5 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_5  HFPP_GET_PAR_BASETYPE_VAL(5)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_5 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_5  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 6
#if HFPP_GET_PAR_BASETYPE_VAL(6) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_6 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_6  HFPP_GET_PAR_BASETYPE_VAL(6)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_6 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_6  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 7
#if HFPP_GET_PAR_BASETYPE_VAL(7) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_7 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_7  HFPP_GET_PAR_BASETYPE_VAL(7)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_7 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_7  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 8
#if HFPP_GET_PAR_BASETYPE_VAL(8) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_8 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_8  HFPP_GET_PAR_BASETYPE_VAL(8)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_8 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_8  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 9
#if HFPP_GET_PAR_BASETYPE_VAL(9) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_9 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_9  HFPP_GET_PAR_BASETYPE_VAL(9)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_9 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_9  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 10
#if HFPP_GET_PAR_BASETYPE_VAL(10) > 0
#define HFPP_PAR_TYPE_IS_TEMPLATED_10 HFPP_TRUE
#define HFPP_GET_PAR_TEMPLATE_VALUE_10  HFPP_GET_PAR_BASETYPE_VAL(10)
#else
#define HFPP_PAR_TYPE_IS_TEMPLATED_10 HFPP_FALSE
#define HFPP_GET_PAR_TEMPLATE_VALUE_10  0
#endif
#endif

#if HFPP_GET_FUNC_PARNUM > 11
------ ATTENTION!!! NUMBER OF PARAMETERS TOO LONG FOR FUNCTION HFPP_GET_FUNC_NAME -------
#endif

//Unless, explicitly set, yields a "return" statement if the function type is not void 
#ifndef HFPP_GET_FUNC_RETURN
#if BOOST_PP_CAT(HFPP_IS_EQUAL,HFPP_GET_FUNC_TYPE) == HFPP_IS_EQUALvoid
#define HFPP_GET_FUNC_RETURN
#else
#define HFPP_GET_FUNC_RETURN return 
#endif
#endif



#ifdef HFPP_VERBOSE
/* hfppnew-generatewrappers.h - DEFAULT Values .................................*/
//
//Decide whether the return value of the function will be adapted to
//the currently wrapped container class or not (only relevant if
//container, e.g. a vector is returned)
/* HFPP_FUNC_BASE_CLASS_ALLOWED_TO_CHANGE= */ HFPP_FUNC_BASE_CLASS_ALLOWED_TO_CHANGE
//
//Define the base types of for the container classes will be made available (int, float, ...)
//For each container class all these types will be instantiated
/* HFPP_WRAPPER_TYPES= */ HFPP_WRAPPER_TYPES
//
//Define the classes of wrappers that will be produced (STL, CASA, ....)
/* HFPP_WRAPPER_CLASSES= */ HFPP_WRAPPER_CLASSES
/* HFPP_MAKE_PYTHONWRAPPERS= */ HFPP_MAKE_PYTHONWRAPPERS
/* HFPP_TEMPLATE_PARAMETER_NAMES= */ HFPP_TEMPLATE_PARAMETER_NAMES
/* HFPP_GET_FUNC_RETURN = */ HFPP_GET_FUNC_RETURN 
/* hfppnew-generatewrappers.h - Conditional Values .............................*/
/* HFPP_GET_FUNC_BASETYPE= */ HFPP_GET_FUNC_BASETYPE
//
/* HFPP_PAR_TYPE_IS_TEMPLATED_0= */ HFPP_PAR_TYPE_IS_TEMPLATED_0
/* HFPP_GET_PAR_BASETYPE_0= */ HFPP_GET_PAR_BASETYPE_0
//
////////////////////////////////////////////////////////////////////////////////
//Generate wrappers
#ifndef HFPP_DONOT_MAKE_WRAPPERS
HFPP_MAKE_WRAPPERS
#endif
////////////////////////////////////////////////////////////////////////////////
//
/* hfppnew-generatewrappers.h - END ............................................*/
#endif

