/*
Test of boost preprocessor programming
test with

gcc -E -I/Users/falcke/LOFAR/usg/release/include hftest-pp.cpp | gawk '/^#/{next} /^[ ]*$/{next} {print}'


Bug fixes:
/Users/falcke/LOFAR/usg/release/include/boost/preprocessor/seq/first_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory
/Users/falcke/LOFAR/usg/release/include/boost/preprocessor/seq/rest_n.hpp:17:52: error: boost/preprocessor/seq/detail/split.hpp: No such file or directory

change to: #include <boost/preprocessor/detail/split.hpp>

*/

#include "hfprep.hpp"

//BOOST_PP_ENUM(HF_NPAR, HF_LISTPAR, data)

//BOOST_PP_TUPLE_ELEM(4,1, HF_PAR0)
//BOOST_PP_SEQ_ELEM(2, HF_PARAMETERS)

//--------------------------------------------------------------------------------
//Now define actual functions
//--------------------------------------------------------------------------------

#define HF_FUNCNAME TEST  //The Name of the function
#define HF_FUNCTYPE void  // Function returns no value
#define HF_NVECS 1 //one input/output vector
#define HF_PARLIST(T)  //no additional paramters

#define HF_NPAR 2
#define HF_PAR0 (freeblocklen,HInteger,128,"Length of a block")
#define HF_PAR1 (blocki,HInteger,128,"Length of a block")
#define HF_PAR2 (blocko,HInteger,128,"Length of a block")

NumPythonWrappers
#undef HF_FUNCNAME
#undef HF_NVECS

#define HF_FUNCNAME TOST  //The Name of the function
#define HF_NVECS 2 //number of input/output vectors
NumPythonWrappers
VecWrappers_1VEC

