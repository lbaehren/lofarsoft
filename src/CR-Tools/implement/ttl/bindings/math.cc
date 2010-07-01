/**************************************************************************
 *  This file is part of the Transient Template Library.                  *
 *  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        * 
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

// SYSTEM INCLUDES
//

// PROJECT INCLUDES
#include <ttl/math.h>

// LOCAL INCLUDES
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_util.h"
#include "numpy_ptr.h"

// FORWARD REFERENCES
//

namespace ttl
{
  namespace bindings
  {

  void sin(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::sin(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void cos(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::cos(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void tan(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::tan(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void sinh(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::sinh(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void cosh(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::cosh(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void tanh(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::tanh(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void asin(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::asin(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void acos(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::acos(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void atan(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::atan(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void log(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::log(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void log10(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::log10(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void logSave(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::logSave(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void abs(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::abs(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void exp(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::exp(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void sqrt(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::sqrt(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void fabs(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::fabs(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void floor(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::floor(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  void ceil(boost::python::numeric::array pydata0, boost::python::numeric::array pydata1)
  {
    ttl::math::ceil(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyBeginPtr<double>(pydata1));
  }

  double norm(boost::python::numeric::array pydata0)
  {
    return ttl::math::norm(ttl::numpyBeginPtr<double>(pydata0), ttl::numpyEndPtr<double>(pydata0));
  }

  }// End bindings
} // End ttl

BOOST_PYTHON_MODULE(math)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  def("sin", ttl::bindings::sin, "Take the sin of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("cos", ttl::bindings::cos, "Take the cos of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("tan", ttl::bindings::tan, "Take the tan of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("sinh", ttl::bindings::sinh, "Take the sinh of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("cosh", ttl::bindings::cosh, "Take the cosh of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("tanh", ttl::bindings::tanh, "Take the tanh of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("asin", ttl::bindings::asin, "Take the asin of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("acos", ttl::bindings::acos, "Take the acos of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("atan", ttl::bindings::atan, "Take the atan of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("log", ttl::bindings::log, "Take the log of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("log10", ttl::bindings::log10, "Take the log10 of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("logSave", ttl::bindings::logSave, "Take the log of all elements in the itterable but return LOW_NUMBER if element is zero\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("abs", ttl::bindings::abs, "Take the abs of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("exp", ttl::bindings::exp, "Take the exp of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("sqrt", ttl::bindings::sqrt, "Take the sqrt of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("fabs", ttl::bindings::fabs, "Take the fabs of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("floor", ttl::bindings::floor, "Take the floor of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("ceil", ttl::bindings::ceil, "Take the ceil of all elements in the itterable\n\n  *begin* begin itterator\n  *end* end itterator\n");
  def("norm", ttl::bindings::norm, "Returns the norm (or length) of a vector\n\n      \\sqrt(\\sum_{i}(x_{i}^2)).\n\n  *vec* \n  *vec_end* \n");
}
