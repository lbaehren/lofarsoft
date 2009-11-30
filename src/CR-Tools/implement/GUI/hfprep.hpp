#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/enum.hpp>
#include <boost/preprocessor/tuple.hpp>
#include <boost/preprocessor/repeat.hpp>


#define HF_NUMERIC_TYPES (HInteger)(HNumber)(HComplex)
#define HF_NON_NUMERIC_TYPES (HPointer)(HString)
#define HF_ALL_TYPES BOOST_PP_CAT(HF_NUMERIC_TYPES,HF_NON_NUMERIC_TYPES)


/*
HF_PythonWrapper_Code: Defines a pointer to the function FUNC with
parameters in HF_PARLIST and template class T. The pointer is assigned
the pointer to the corresponding function and later be given to the
Python function that will call it. Through this assignment the
function will also be instantiated with type T!

We can also create dummy functions with HF_Make_PythonWrappers_Dummy
which creates just empty functions of name FUNC and class type T. This
is used to instantiate certain templates that are needed by the GUI
for formal reasons (i.e. typically HString and HPointer functions).

 */

//Parameter list with only one input/output vector
#define HF_PARLIST_1VECSTL(T) vector<T> &vec0
//Parameter list with two input/output vectors
#define HF_PARLIST_2VECSTL(T) vector<T> &vec0, vector<T> &vec1
//Parameter list with oner or two casa input/output vectors
#define HF_PARLIST_1VECCASA(T) casa::Vector<T> &vec0
#define HF_PARLIST_2VECCASA(T) casa::Vector<T> &vec0, casa::Vector<T> &vec1

//Create a vector parameterlist based on the number of vector NVECS and the type VECTYPE (=STL or CASA)
#define HF_VECLIST(T,VECTYPE) BOOST_PP_CAT(BOOST_PP_CAT(HF_PARLIST_,HF_NVECS),BOOST_PP_CAT(VEC,VECTYPE)(T))
//Create the entire parameter list consisting of the vectors and the other parameters
#define HF_MAKE_PARLIST(T,VECTYPE) HF_VECLIST(T,STL) HF_PARLIST(T) 


#define HF_PythonWrapper_Code(R,FUNC,T) HF_FUNCTYPE (*BOOST_PP_CAT(FUNC,T))(HF_MAKE_PARLIST(T,STL)) = &FUNC; 
#define HF_PythonWrapper_Dummy_Code(R,FUNC,T) HF_FUNCTYPE FUNC (HF_MAKE_PARLIST(T,STL)) {}
#define HF_Make_PythonWrappers(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HF_PythonWrapper_Code,FUNC,TYPE_LIST)
#define HF_Make_PythonWrappers_Dummy(FUNC,TYPE_LIST) BOOST_PP_SEQ_FOR_EACH(HF_PythonWrapper_Dummy_Code,FUNC,TYPE_LIST)
#define NumPythonWrappers \
  HF_Make_PythonWrappers(HF_FUNCNAME,HF_NUMERIC_TYPES) \
  HF_Make_PythonWrappers_Dummy(HF_FUNCNAME,HF_NON_NUMERIC_TYPES) 

#define HF_WRAPPER_CODE_STL
#define HF_WRAPPER_CODE_CASA

#define VecWrappers_1VEC			\
  template <class T> inline HF_FUNCTYPE HF_FUNCNAME(HF_MAKE_PARLIST(T,STL)) {HF_WRAPPER_CODE_STL return HF_FUNCNAME(vec0.begin(),vec0.end());} \
  template <class T> inline HF_FUNCTYPE HF_FUNCNAME(HF_MAKE_PARLIST(T,CASA)) {HF_WRAPPER_CODE_CASA return HF_FUNCNAME(vec0.cbegin(),vec0.cend());} 
#define VecWrappers_2VEC			\
  template <class T> inline HF_FUNCTYPE HF_FUNCNAME(HF_MAKE_PARLIST(T,STL)) {HF_WRAPPER_CODE_STL return HF_FUNCNAME(vec0.begin(),vec0.end(),vec1.begin(),vec1.end());} \
  template <class T> inline HF_FUNCTYPE HF_FUNCNAME(HF_MAKE_PARLIST(T,CASA)) {HF_WRAPPER_CODE_CASA return HF_FUNCNAME(vec0.cbegin(),vec0.cend(),vec1.cbegin(),vec1.cend());} 


#define HF_LISTPAR_DEF(count,VAL,DATA)  BOOST_PP_TUPLE_ELEM(4,1,BOOST_PP_CAT(HF_PAR,VAL)) BOOST_PP_TUPLE_ELEM(4,0,BOOST_PP_CAT(HF_PAR,VAL)) 
#define HF_LISTPAR_CALL(count,VAL,DATA)  BOOST_PP_TUPLE_ELEM(4,0,BOOST_PP_CAT(HF_PAR,VAL)) 




