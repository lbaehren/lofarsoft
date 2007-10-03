#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle

#include <boost/python.hpp>
#include <num_util.h>

#include "stat.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(_cbdsm)
{
  import_array();
  numeric::array::set_module_and_type("numpy", "ndarray");

  def("bstat", &bstat);
}
