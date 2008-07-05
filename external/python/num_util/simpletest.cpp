#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_util.h"
#include <iostream>

namespace { const char* rcsid = "$Id: simpletest.cpp 39 2007-02-01 02:54:54Z phil $"; }

namespace bp=boost::python;
namespace ar=boost::python::numeric;
namespace nbpl = num_util;

//send a dictionary with two arrays back to python
bp::dict testToPython(void){
  double ia[6] = {1,2,3,4,5,6};
  int ib[3] = {88,99,100};
  //construct two numpy arrays
  ar::array pyDoubles=nbpl::makeNum(ia,6);
  ar::array pyInts=nbpl::makeNum(ib,3);
  //pack them in a dictionary and return to python
  bp::dict retvals;
  retvals["myDoubles"]=pyDoubles;
  retvals["myInts"]=pyInts;
  return retvals;
}

//get an array from python
void testFromPython(ar::array inValue){
  nbpl::check_type(inValue, PyArray_DOUBLE);
  nbpl::check_rank(inValue, 2);
  double* dataPtr = (double*) nbpl::data(inValue);
  int theSize= nbpl::size(inValue);
  std::cout << std::endl << "data values on c++ side: " << std::endl;
  for(int i=0;i < theSize;++i){
     std::cout << *(dataPtr + i) << std::endl;
  }
}  

void testFromPython2(ar::array inValue){
  nbpl::check_type(inValue, PyArray_DOUBLE);
  nbpl::check_rank(inValue, 2);
  double* dataPtr = (double*) nbpl::data(inValue);
  int theSize= nbpl::size(inValue);
  std::cout << std::endl << "data values on c++ side: " << std::endl;
  for(int i=0;i < theSize;++i){
     std::cout << *(dataPtr + i) << std::endl;
  }
}

  
BOOST_PYTHON_MODULE(simple_ext)
{ 
  import_array();
  ar::array::set_module_and_type("numpy", "ndarray");
  //global doc string
  bp::scope().attr("RCSID") = rcsid;
  std::string str1;
  str1="docstring for module\n";
  bp::scope().attr("__doc__") = str1.c_str();
  str1 = "Doc string for testToPython\n";
  def("testToPython", testToPython,str1.c_str());
  str1 = "Doc string for testFromPython\n";
  def("testFromPython", testFromPython,str1.c_str());
  str1 = "Doc string for testFromPython2\n";
  def("testFromPython2", testFromPython2,str1.c_str());
}
