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
#include <ttl/fft.h>

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

    bool forwardFFTW(boost::python::numeric::array out, boost::python::numeric::array in)
    {
      return ttl::fft::forwardFFTW(ttl::numpyBeginPtr< std::complex<double> >(out), ttl::numpyBeginPtr<double>(in));
    }

    } // End bindings
} // End ttl

BOOST_PYTHON_MODULE(fft)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  boost::python::register_exception_translator<ttl::TypeError>(ttl::exceptionTranslator);

  def("forwardFFTW", ttl::bindings::forwardFFTW, "Calculates the forward FFT\n\n*out* output array\n*in* input array\n");
}

