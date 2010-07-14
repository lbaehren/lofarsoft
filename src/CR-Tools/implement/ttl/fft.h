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

#ifndef TTL_FFT_H
#define TTL_FFT_H

// SYSTEM INCLUDES
#include <cmath>
#include <complex>
#include <iostream>

// PROJECT INCLUDES
//

// LOCAL INCLUDES
#include <fftw3.h>

// FORWARD REFERENCES
//

// DOXYGEN TAGS
/*!
  \file fft.h
  \ingroup CR
  \ingroup CR_TTL
 */

namespace ttl
{
  namespace fft
  {
    /*!
      \brief Calculates the forward FFT real -> complex
      
      \param out output array
      \param in input array

      This implementation uses fftw3 - for more information see: http://www.fftw.org/fftw3.pdf
      
      The DFT results are stored in-order in the array out, with the
      zero-frequency (DC) component in data_out[0]. 

      Users should note that FFTW computes an unnormalized DFT. Thus,
      computing a forward followed by a backward transform (or vice versa)
      results in the original array scaled by N.
      
      The size N can be any positive integer, but sizes that are products of
      small factors are transformed most efficiently (although prime sizes
      still use an O(N log N) algorithm).
    */
    template <class CIter, class DIter>
      bool forwardFFTW(CIter out, CIter out_end,
		       DIter in, DIter in_end)
    {
      int N = std::distance(in, in_end);
      int Nout = std::distance(out, out_end);
      if (Nout != (N/2+1)) {
	std::cerr << "forwardFFTW" << ": input and output vector have wrong sizes! N(in)=" << N  << " N(out)=" << Nout<< " (should be = " << N/2+1 << ")" << std::endl;
	return false;
      };
      fftw_plan p;
      p = fftw_plan_dft_r2c_1d(N, &(*in), (fftw_complex*) &(*out), FFTW_ESTIMATE);
      fftw_execute(p);
      fftw_destroy_plan(p);
      
      return true;
    }
    
    /*!
      \brief Calculates the backward FFT complex -> real
      
      \param out output array
      \param in input array
      
      Users should note that FFTW computes an unnormalized DFT. Thus,
      computing a forward followed by a backward transform (or vice versa)
      results in the original array scaled by N.
    */
    template <class DIter, class CIter>
      bool backwardFFTW(DIter out, DIter out_end,
			CIter in,  CIter in_end)
    {
      int Nin = std::distance(in, in_end);
      int N = std::distance(out, out_end);
      if (Nin != (N/2+1)) {
	std::cerr << "backwardFFTW" << ": input and output vector have wrong sizes! N(out)=" << N  << " N(in)=" << Nin<< " (should be = " << N/2+1 << ")" << std::endl;
	return false;
      };
      
      fftw_plan p;
      p = fftw_plan_dft_c2r_1d(N, (fftw_complex*) &(*in),  &(*out), FFTW_ESTIMATE|FFTW_PRESERVE_INPUT);
      fftw_execute(p); 
      fftw_destroy_plan(p);
            
      return true;
    }
  } // End fft
} // End ttl

#endif // TTL_FFT_H

