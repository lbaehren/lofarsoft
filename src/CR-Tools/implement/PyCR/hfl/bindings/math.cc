/**************************************************************************
 *  This file is part of the Heino Falcke Library.                        *
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
#include <hfl/math.h>

// LOCAL INCLUDES
#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle
#include "num_util.h"
#include "numpy_ptr.h"

// FORWARD REFERENCES
//

namespace hfl
{
    namespace bindings
    {

    void sin(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::sin(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::sin(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void cos(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::cos(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::cos(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void tan(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::tan(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::tan(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void sinh(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::sinh(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::sinh(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void cosh(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::cosh(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::cosh(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void tanh(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::tanh(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::tanh(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void asin(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::asin(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::asin(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void acos(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::acos(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::acos(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void atan(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::atan(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::atan(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void log(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::log(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::log(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void log10(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::log10(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::log10(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void logSave(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::logSave(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::logSave(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void abs(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::abs(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::abs(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void exp(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::exp(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::exp(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void sqrt(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::sqrt(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::sqrt(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void fabs(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::fabs(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::fabs(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void floor(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::floor(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::floor(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    void ceil(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          hfl::math::ceil(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          hfl::math::ceil(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    double norm(boost::python::numeric::array vec)
    {
      switch (num_util::type(vec))
      {
        case PyArray_FLOAT:
        {
          return hfl::math::norm(hfl::numpyBeginPtr<float>(vec), hfl::numpyEndPtr<float>(vec));
        }
        break;
        case PyArray_DOUBLE:
        {
          return hfl::math::norm(hfl::numpyBeginPtr<double>(vec), hfl::numpyEndPtr<double>(vec));
        }
        break;
        default:
        {
          throw hfl::TypeError();
        }
        break;
      }
    }

    } // End bindings
} // End hfl

BOOST_PYTHON_MODULE(math)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  boost::python::register_exception_translator<hfl::TypeError>(hfl::exceptionTranslator);

  def("sin", hfl::bindings::sin, "Take the sin of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("cos", hfl::bindings::cos, "Take the cos of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("tan", hfl::bindings::tan, "Take the tan of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("sinh", hfl::bindings::sinh, "Take the sinh of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("cosh", hfl::bindings::cosh, "Take the cosh of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("tanh", hfl::bindings::tanh, "Take the tanh of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("asin", hfl::bindings::asin, "Take the asin of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("acos", hfl::bindings::acos, "Take the acos of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("atan", hfl::bindings::atan, "Take the atan of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("log", hfl::bindings::log, "Take the log of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("log10", hfl::bindings::log10, "Take the log10 of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("logSave", hfl::bindings::logSave, "Take the log of all elements in the itterable but return LOW_NUMBER if element is zero\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("abs", hfl::bindings::abs, "Take the abs of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("exp", hfl::bindings::exp, "Take the exp of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("sqrt", hfl::bindings::sqrt, "Take the sqrt of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("fabs", hfl::bindings::fabs, "Take the fabs of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("floor", hfl::bindings::floor, "Take the floor of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("ceil", hfl::bindings::ceil, "Take the ceil of all elements in the itterable\n\n*vec* begin iterator\n*vec_end* end iterator\n");
  def("norm", hfl::bindings::norm, "Returns the norm (or length) of a vector\n\n      \\sqrt(\\sum_{i}(x_{i}^2)).\n\n*vec* begin iterator\n*vec_end* end iterator\n");
}

