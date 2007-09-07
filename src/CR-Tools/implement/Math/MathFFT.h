/*-------------------------------------------------------------------------*
 | $Id::                                                                 $ |
 *-------------------------------------------------------------------------*
 ***************************************************************************
 *   Copyright (C) 2004-2006                                               *
 *   Lars B"ahren (bahren@astron.nl)                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef MATHFFT_H
#define MATHFFT_H

// AIPS++ wrapper classes
#include <casa/aips.h>
#include <casa/string.h>
#include <casa/Arrays.h>
#include <casa/Arrays/Array.h>
#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Matrix.h>
#include <casa/Arrays/Vector.h>
#include <casa/BasicMath/Math.h>
#include <casa/BasicSL/Complex.h>
#include <casa/OS/Time.h>
#include <casa/Quanta.h>
#include <scimath/Mathematics.h>
#include <scimath/Mathematics/FFTServer.h>
#include <measures/Measures.h>

// Custom header files
#include <Utilities/ProgressBar.h>

#include <casa/namespace.h>

namespace CR { // Namespace CR -- begin
  
  /*!
    \brief Fourier transform related mathematical operations.
    
    \ingroup Math
    
    \author Lars B&auml;hren
    
    \date 2005/02
    
    This class provides a collection of mathematical tools and functions for
    Fourier transform related mathematical operations.
    
    <h3>Prerequisite</h3>
    
    <ul>
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Array.html">Array</a>
      A templated N-D Array class with zero origin.
      <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Vector.html">Vector</a>,
      <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/Matrix.html">Matrix</a>,
      and Cube are one, two, and three dimensional specializations of Array.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Arrays/ArrayMath.html">Mathematical
      operations for Arrays</a>.
      <li>[AIPS++] <a href="http://aips2.nrao.edu/docs/aips/implement/Mathematics/FFTServer.html">FFTServer</a>
      -- Methods for Fast Fourier Transforms. The FFTServer
      class provides methods for performing n-dimensional Fast Fourier Transforms
      with real and complex Array's of arbitrary size and dimensionality. It can
      do either real to complex, complex to real, or complex to complex transforms
      with the "origin" of the transform either at the centre of the Array or at 
      the first element.<br>
      Also see the description of the
      <a href="http://aips2.nrao.edu/docs/user/Utility/node287.html">fftserver--Tool</a>
    </ul>
    
    <h3>References</h3>
    
    <ul>
    <li>Mathworld (Wolfram Research), <i>Cross-Correlation</i>
    (http://mathworld.wolfram.com/Cross-Correlation.html)
    <li>Numerical Recipes in C (http://www.library.cornell.edu/nr/bookcpdf.html)
    <li>P.J. Lewis, <i>Fast Normalized Cross-Correlation</i>
    (http://www.idiom.com/~zilla/Papers/nvisionInterface/nip.html)
    <li>Chatfield, C. The analysis of time series. Chapman and Hall, 1984.
    <li>Julius O. Smith, <i>Mathematics of the Discrete Fourier Transform (DFT)</i>
    (http://ccrma-www.stanford.edu/~jos/mdft/Cross_Correlation.html)
  </ul>

*/

  /*!
    \brief Compute a new time-series from fft-data 
    
    Origin: <tt>data-func.g</tt>
    
    Compute a new time-series from Fourier transformed data (i.e. data in the
    frequency domain).
    
    The origin of the transform is defined as the point where setting only that
    element to one, and then doing a forward transform results in an Array that
    is all one. The fft member functions in FFTServer all assume that the origin
    of the Transform is at the centre of the Array ie. at [nx/2,ny/2,...] were
    the indexing begins at zero.
    
    Based on the AIPS++ FFT server class, two different types of FFTs are
    supported:
    - <i>REALTOCOMPLEX</i> -- Real to Complex (forward) or Complex to Real
                              (backward) transforms.
    - <i>COMPLEX</i> -- Complex to Complex transforms.
    Though both methods are supported, tests indicate a higher accuracy in the
    case of the Complex to Complex transform.
    
    \param from          -- Array with the original FFT-data
    \param fftsize       -- Size of the dataset, that was fft'ed
    \param invdir        -- Invert contents of the FFT-data?
    \param realtocomplex -- Use 'REALTOCOMPLEX' version of FFT server; if 'FALSE'
                            use 'COMPLEX'
  */
  template <typename T, typename S>
    casa::Vector<T> FFT2Fx (casa::Vector<S> &from,
			    Int const &fftsize,
			    bool const &invdir=true,
			    bool const &realtocomplex=false);
  
  /*!
    \brief Compute time-series from FFT'ed data
    
    \param fft       -- Array with the original FFT-data
    \param fftsize   -- Size of the dataset, that was fft'ed
  */
  template <typename T, typename S>
    casa::Vector<T> backwardsFFT (casa::Vector<S>& fft,
				  Int const &fftsize);
  
} // end: Namespace Math

#endif
  
