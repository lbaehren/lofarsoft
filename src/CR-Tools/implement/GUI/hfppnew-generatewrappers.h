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

#ifndef HFPP_WRAPPER_LIST
//Defines all the different types of wrappers that will be built (STL, CASA, ...)
#define HFPP_WRAPPER_LIST HFPP_DEFAULT_WRAPPER_LIST
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
#define HFPP_FUNC_TYPE_IS_TEMPLATED 1
#define HFPP_GET_FUNC_BASETYPE BOOST_PP_SEQ_ELEM(HFPP_GET_FUNC_TYPE_VALUE,HFPP_TEMPLATE_PARAMETER_NAMES) 
#else
#define HFPP_FUNC_TYPE_IS_TEMPLATED 0
#define HFPP_GET_FUNC_BASETYPE HFPP_GET_FUNC_TYPE_VALUE
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
/* HFPP_WRAPPER_LIST= */ HFPP_WRAPPER_LIST
/* HFPP_TEMPLATE_PARAMETER_NAMES= */ HFPP_TEMPLATE_PARAMETER_NAMES
/* HFPP_GET_FUNC_RETURN = */ HFPP_GET_FUNC_RETURN 
/* hfppnew-generatewrappers.h - Conditional Values .............................*/
/* HFPP_GET_FUNC_BASETYPE= */ HFPP_GET_FUNC_BASETYPE
/* hfppnew-generatewrappers.h - END ............................................*/
#endif
