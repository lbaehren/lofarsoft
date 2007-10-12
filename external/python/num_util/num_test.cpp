#include "num_test.h"
#define NO_IMPORT_ARRAY
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_util.h"

namespace { const char* rcsid = "$Id: num_test.cpp 46 2007-02-01 03:06:40Z phil $"; } 

namespace py = boost::python;
namespace nbpl = num_util;

namespace num_test {

py::numeric::array test_1d_float_construct( py::numeric::array arr ) {
  return nbpl::makeNum((float *)nbpl::data(arr), nbpl::size(arr));
}
  
py::numeric::array test_nd_double_construct( py::numeric::array arr ) {
  return nbpl::makeNum((double *)nbpl::data(arr), nbpl::shape(arr));
}
  
py::numeric::array test_copy_construct( py::numeric::array arr ) {
  return nbpl::makeNum(arr);
}
  
py::numeric::array test_int_type_construct( int size, char e_type ) {
  return nbpl::makeNum(size, nbpl::char2type(e_type));
}

py::numeric::array test_dims_type_construct( py::tuple e_dims, char e_type ) {
  int tup_size = PyTuple_Size(e_dims.ptr());
  std::vector<int> dims;
  for (int i=0; i<tup_size; i++) {
    dims.push_back( py::extract<int>(e_dims[i]) );
  }
  return nbpl::makeNum(dims, nbpl::char2type(e_type));
}

py::numeric::array test_clone( py::numeric::array arr ) {
  return nbpl::clone(arr);
}

py::numeric::array test_astype( py::numeric::array arr, char e_type ) {
  return nbpl::astype(arr, nbpl::char2type(e_type));
}

void test_checks( py::numeric::array arr, py::tuple e_dims, 
		  int e_rank, int e_size, char e_type ) {
  std::vector<int> dims;
  int rank;
  nbpl::check_contiguous(arr);
  rank = nbpl::rank(arr);
  for (int i=0; i<rank; i++) {
    dims.push_back( py::extract<int>(e_dims[i]) );
  }
  nbpl::check_shape(arr, dims);
  nbpl::check_rank(arr, e_rank);
  nbpl::check_size(arr, e_size);
  nbpl::check_type(arr, nbpl::char2type(e_type));
  return;
}

py::list test_data_float( py::numeric::array arr ) {
  double * dataPtr = (double*) nbpl::data(arr);
  py::list results;
  for (int i=0; i<nbpl::size(arr); i++) {
    results.append(dataPtr[i]);
  }
  return results;
}

py::list test_stats( py::numeric::array arr ) {
  std::vector<int> dims, theStrides;
  py::list results;
  int rank;
  results.append("type");
  results.append(nbpl::type2string(nbpl::type(arr)));
  results.append("rank");
  rank = nbpl::rank(arr);
  results.append(rank);
  results.append("size");
  results.append(nbpl::size(arr));
  results.append("refcount");
  results.append(nbpl::refcount(arr));
  results.append("dimension sizes");
  dims = nbpl::shape(arr);
  for (int i=0; i<rank; i++) {
    results.append(i+1);
    results.append(dims.back());
    dims.pop_back();
  }
  results.append("contiguous");
  if ( nbpl::iscontiguous(arr) )
    results.append(1);
  else
    results.append(0);
  results.append("strides");
  theStrides = nbpl::strides(arr);
  for (int i=0; i<rank; i++) {
    results.append(i+1);
    results.append(theStrides.back());
    theStrides.pop_back();
  }
  return results;
}


} //namespace num_test



