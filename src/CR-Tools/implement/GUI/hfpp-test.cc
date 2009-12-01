/*
Test of boost preprocessor programming
test with

gcc -E -I/Users/falcke/LOFAR/usg/release/include hfpp-test.cc | gawk '/^#/{next} /^[ ]*$/{next} {print}'


Bug fixes:
/Users/falcke/LOFAR/usg/release/include/boost/preprocessor/seq/first_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory
/Users/falcke/LOFAR/usg/release/include/boost/preprocessor/seq/rest_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory

change to: #include <boost/preprocessor/detail/split.hpp>

*/
//--------------------------------------------------------------------------------
#include <GUI/hfprep.hpp>
// --------------------------------------------------------------------------------
// kept the two files in one for testing. Later the part below until
// "END hfprep.hpp" should be put in a separate file



//--------------------------------------------------------------------------------
//Now define actual functions
//--------------------------------------------------------------------------------
#include "hfpp-undef.cc"
//$FILENAME: HFILE=$SELF.h
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
/*
HF_PP_VEC_WRAPPER_CODE_STL
HF_PP_VEC_WRAPPER_CODE_CASA

does something like

return TEST( vec0.begin(),vec0.end() , vec1.begin(),vec1.end(), var_a , var_b , var_c)
return TEST( vec0.cbegin(),vec0.cend() , vec1.cbegin(),vec1.cend(), var_a , var_b , var_c)
*/

//$WRITE_TO HFILE: HF_PP_GENERATE_WRAPPERS_hFILE 
/*Now undefine the definitions above*/
#include "hfpp-undef.cc"
//$WRITE_TO HFILE END --------------------------------------------------

//$COPY_TO_HFILE START --------------------------------------------------
#define HF_PP_FUNCNAME FOO  //The Name of the function
#define HF_PP_FUNCTYPE HInteger  //Return value type of function
#define HF_PP_NVECS 1 //Number of (input/output) vectors

#define HF_PP_PAR0 (para,HInteger,128,"Parameter A")  //Definition of one input parameter
#define HF_PP_NPAR 1                                  //Number of scalar parameters

#define HF_PP_VEC_WRAPPER_CODE_STL return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(STL),HF_PP_PAR_PARAMETERS)
#define HF_PP_VEC_WRAPPER_CODE_CASA return HF_PP_FUNCNAME(HF_PP_PAR_VECTORITERATORS(CASA),HF_PP_PAR_PARAMETERS)

HF_PP_GENERATE_WRAPPERS 
#include "hfpp-undef.cc"
//$COPY_TO_HFILE END --------------------------------------------------

