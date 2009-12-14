/*
Test of boost preprocessor programming
test with

gcc -E -I$LOFARSOFT/release/include hfpp-test.cc | gawk '/^#/{next} /^[ ]*$/{next} {print}'

or for an actual file 
gcc -E -I$LOFARSOFT/release/include -I$LOFARSOFT/src/CR-Tools/implement hfanalysis.cc | gawk '/^#/{next} /^[ ]*$/{next} {print}' > tst

gcc -E -I$LOFARSOFT/release/include -I$LOFARSOFT/src/CR-Tools/implement ~/LOFAR/usg/build/cr/implement/GUI/hffuncs.awk.cc  | gawk '/^#/{next} /^[ ]*$/{next} {print}' > tst

gcc -E -I$LOFARSOFT/release/include -I$LOFARSOFT/src/CR-Tools/implement -I$LOFARSOFT/build/cr/implement/GUI/ -I$LOFARSOFT/build/cr/implement -I. ~/LOFAR/usg/build/cr/implement/GUI/hffuncs.awk.cc  | gawk '/^#/{next} /^[ ]*$/{next} {print}' > tst

gcc -E -I$LOFARSOFT/release/include -I$LOFARSOFT/src/CR-Tools/implement -I$LOFARSOFT/build/cr/implement/GUI/ -I$LOFARSOFT/build/cr/implement -I. ~/LOFAR/usg/src/CR-Tools/implement/GUI/hfanalysis.cc  | gawk '/^#/{next} /^[ ]*$/{next} {print}' > tst


Bug fixes:
$LOFARSOF/release/include/boost/preprocessor/seq/first_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory
$LOFARSOF/release/include/boost/preprocessor/seq/rest_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory

change to: #include <boost/preprocessor/detail/split.hpp>

*/
//--------------------------------------------------------------------------------
#include "hfpp.h"
#define HF_PP_FILETYPE() (CC)  // Tell the preprocessor that this is a c++ source code file
//#define HF_PP_FILETYPE hFILE  // Tell the preprocessor that this is a header file
//#define HF_PP_FILETYPE hPYTHON  // Tell the preprocessor that this is a header file for Python expose
// --------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//Now define actual functions
//--------------------------------------------------------------------------------


//$FILENAME: HFILE=$SELF.h
//$COPY_TO HFILE START:
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME TEST  //The Name of the function
#define HF_PP_FUNCTYPE void  //Return value type of function
#define HF_PP_NVECS 2 //Number of (input/output) vectors
#define HF_PP_NPAR 3 //Number of scalar parameters

//Parameter definiton is ordered as (name, Type    , default, Description, templated type T, pass-as-reference/return value)

#define HF_PP_PAR0 (var_a, HInteger, 128, "Length of a block",0,0) //Input Parameter
#define HF_PP_PAR1 (var_b,        T, 128, "Length of a block",1,0)
#define HF_PP_PAR2 (var_c, HInteger, 128, "Length of a block",0,1)

//Now define the c++ commands that are executed in order to call the underlying function
#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(STL),HF_PP_PAR_PARAMETERS)
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(CASA),HF_PP_PAR_PARAMETERS)
/*
HF_PP_VEC_WRAPPER_CODE_STL
HF_PP_VEC_WRAPPER_CODE_CASA

does something like

return TEST( vec0.begin(),vec0.end() , vec1.begin(),vec1.end(), var_a , var_b , var_c)
return TEST( vec0.cbegin(),vec0.cend() , vec1.cbegin(),vec1.cend(), var_a , var_b , var_c)
*/
HF_PP_PYTHON_EXPOSE
HF_PP_GENERATE_WRAPPERS 
//$COPY_TO HFILE END --------------------------------------------------

//$COPY_TO HFILE START --------------------------------------------------
#include "hfpp-undef.cc"
#define HF_PP_FUNCNAME FOO  //The Name of the function
#define HF_PP_VARIANT 2 // Define if this is an overload 
#define HF_PP_FUNCBRIEF "Fills a vector with a constant value."
#define HF_PP_FUNCTYPE_T 1  //Return value type of function is templated with type T of vector
#define HF_PP_FUNCTYPE HNumber
#define HF_PP_NVECS 1 //Number of (input/output) vectors
#define HF_PP_GUI_BUFFERED 1
#define HF_PP_GUI_LIBRARY Math

#define HF_PP_PAR0 (para,HInteger,128,"Parameter A",HF_PP_TEMPLATED_TYPE,HF_PP_PASS_AS_VALUE) //Definition of one input parameter
#define HF_PP_NPAR 1                                  //Number of scalar parameters

#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(STL),HF_PP_PAR_PARAMETERS)
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(CASA),HF_PP_PAR_PARAMETERS)

HF_PP_GENERATE_WRAPPERS

HF_PP_GENERATE_WRAPPERS_hFILE
HF_PP_GENERATE_WRAPPERS_PYTHON

GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
#define HF_PP_GUI_RETURN_POLICY 0

hhhhhhhhhhh

HF_PP_GENERATE_WRAPPERS_GUI


//$COPY_TO HFILE END --------------------------------------------------

