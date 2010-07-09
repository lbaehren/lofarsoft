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
      return ttl::fft::forwardFFTW(ttl::numpyBeginPtr< std::complex<double> >(out), ttl::numpyEndPtr< std::complex<double> >(out), ttl::numpyBeginPtr<double>(in), ttl::numpyEndPtr<double>(in));
    }

    bool backwardFFTW(boost::python::numeric::array out, boost::python::numeric::array in)
    {
      return ttl::fft::backwardFFTW(ttl::numpyBeginPtr<double>(out), ttl::numpyEndPtr<double>(out), ttl::numpyBeginPtr< std::complex<double> >(in), ttl::numpyEndPtr< std::complex<double> >(in));
    }

    } // End bindings
} // End ttl

BOOST_PYTHON_MODULE(fft)
{
  import_array();
  boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

  using namespace boost::python;

  boost::python::register_exception_translator<ttl::TypeError>(ttl::exceptionTranslator);

  def("forwardFFTW", ttl::bindings::forwardFFTW, "Calculates the forward FFT real -> complex\n\n\n      This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf\n\n      The DFT results are stored in-order in the array out, with the\n      zero-frequency (DC) component in data_out[0].\n\n      Users should note that FFTW computes an unnormalized DFT. Thus,\n      computing a forward followed by a backward transform (or vice versa)\n      results in the original array scaled by N.\n\n      The size N can be any positive integer, but sizes that are products of\n      small factors are transformed most efficiently (although prime sizes\n      still use an O(N log N) algorithm).\n\n*out* output array\n*out_end* \n*in* input array\n*in_end* \n");
  def("backwardFFTW", ttl::bindings::backwardFFTW, "Calculates the backward FFT complex -> real\n\n\n      Users should note that FFTW computes an unnormalized DFT. Thus,\n      computing a forward followed by a backward transform (or vice versa)\n      results in the original array scaled by N.\n\n*out* output array\n*out_end* \n*in* input array\n*in_end* \n");
}

