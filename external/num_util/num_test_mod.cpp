#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_test.h"
#include "num_util.h"

namespace { const char* rcsid = "$Id: num_test_mod.cpp 39 2007-02-01 02:54:54Z phil $"; }

namespace nbpl = num_util;

namespace num_test {

BOOST_PYTHON_MODULE(num_test_ext)
{ 
  using namespace boost::python;
  import_array();
  numeric::array::set_module_and_type("numpy", "ndarray");

  def("test_1d_float_construct", test_1d_float_construct);
  def("test_nd_double_construct", test_nd_double_construct);
  def("test_copy_construct", test_copy_construct);
  def("test_int_type_construct", test_int_type_construct);
  def("test_dims_type_construct", test_dims_type_construct);
  def("test_clone", test_clone);
  def("test_astype", test_astype);
  def("test_checks", test_checks);
  def("test_stats", test_stats);
  def("test_data_float", test_data_float);
  //def("toggle_spacesave", toggle_spacesave);
  def("refcount", nbpl::refcount);
}

} //namespace num_test
