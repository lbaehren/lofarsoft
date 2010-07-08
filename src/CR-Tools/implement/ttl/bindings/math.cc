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

  void sin(boost::python::numeric::array vec)
  {
    switch (num_util::type(vec))
    {
      case PyArray_INT:
      {
        ttl::math::sin(ttl::numpyBeginPtr<int>(vec), ttl::numpyEndPtr<int>(vec));
        break;
      }
      case PyArray_LONG:
      {
        ttl::math::sin(ttl::numpyBeginPtr<long>(vec), ttl::numpyEndPtr<long>(vec));
        break;
      }
      case PyArray_FLOAT:
      {
        ttl::math::sin(ttl::numpyBeginPtr<float>(vec), ttl::numpyEndPtr<float>(vec));
        break;
      }
      case PyArray_DOUBLE:
      {
        ttl::math::sin(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
        break;
      }
      default:
      {
        break;
      }
    }
  }

  void cos(boost::python::numeric::array vec)
  {
    ttl::math::cos(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void tan(boost::python::numeric::array vec)
  {
    ttl::math::tan(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void sinh(boost::python::numeric::array vec)
  {
    ttl::math::sinh(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void cosh(boost::python::numeric::array vec)
  {
    ttl::math::cosh(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void tanh(boost::python::numeric::array vec)
  {
    ttl::math::tanh(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void asin(boost::python::numeric::array vec)
  {
    ttl::math::asin(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void acos(boost::python::numeric::array vec)
  {
    ttl::math::acos(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void atan(boost::python::numeric::array vec)
  {
    ttl::math::atan(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void log(boost::python::numeric::array vec)
  {
    ttl::math::log(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void log10(boost::python::numeric::array vec)
  {
    ttl::math::log10(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void logSave(boost::python::numeric::array vec)
  {
    ttl::math::logSave(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void abs(boost::python::numeric::array vec)
  {
    ttl::math::abs(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void exp(boost::python::numeric::array vec)
  {
    ttl::math::exp(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void sqrt(boost::python::numeric::array vec)
  {
    ttl::math::sqrt(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void fabs(boost::python::numeric::array vec)
  {
    ttl::math::fabs(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void floor(boost::python::numeric::array vec)
  {
    ttl::math::floor(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  void ceil(boost::python::numeric::array vec)
  {
    ttl::math::ceil(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  double norm(boost::python::numeric::array vec)
  {
    return ttl::math::norm(ttl::numpyBeginPtr<double>(vec), ttl::numpyEndPtr<double>(vec));
  }

  }// End bindings
} // End ttl

BOOST_PYTHON_MODULE(math)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  def("sin", ttl::bindings::sin, "Take the sin of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("cos", ttl::bindings::cos, "Take the cos of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("tan", ttl::bindings::tan, "Take the tan of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("sinh", ttl::bindings::sinh, "Take the sinh of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("cosh", ttl::bindings::cosh, "Take the cosh of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("tanh", ttl::bindings::tanh, "Take the tanh of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("asin", ttl::bindings::asin, "Take the asin of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("acos", ttl::bindings::acos, "Take the acos of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("atan", ttl::bindings::atan, "Take the atan of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("log", ttl::bindings::log, "Take the log of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("log10", ttl::bindings::log10, "Take the log10 of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("logSave", ttl::bindings::logSave, "Take the log of all elements in the itterable but return LOW_NUMBER if element is zero\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("abs", ttl::bindings::abs, "Take the abs of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("exp", ttl::bindings::exp, "Take the exp of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("sqrt", ttl::bindings::sqrt, "Take the sqrt of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("fabs", ttl::bindings::fabs, "Take the fabs of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("floor", ttl::bindings::floor, "Take the floor of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("ceil", ttl::bindings::ceil, "Take the ceil of all elements in the itterable\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
  def("norm", ttl::bindings::norm, "Returns the norm (or length) of a vector\n\n      \\sqrt(\\sum_{i}(x_{i}^2)).\n\n  *vec* begin iterator\n  *vec_end* end iterator\n");
}
