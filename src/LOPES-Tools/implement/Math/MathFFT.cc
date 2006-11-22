/***************************************************************************
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

#include <Math/MathFFT.h>

namespace LOPES { // Namespace LOPES -- begin
  
  // =============================================================================
  //
  //  Fourier transform related operations
  //
  // =============================================================================
  
  template <typename T, typename S>
  Vector<T> FFT2Fx (Vector<S>& from,
		    int const &fftsize,
		    bool const &invdir,
		    bool const &realtocomplex)
  {
    int fftlen (0);
    int halfsize (0);
    Vector<T> dataFx;
    int k (0);
    
    // Adjust the array dimensions
    from.shape(fftlen);
    halfsize = int((fftsize)/2);
    
    // Avoid inversion of the input vector in the FFT
    if (!invdir) {
      Vector<S> tmp(fftlen);
      for (k=0; k<fftlen; k++) {
	tmp(k) = from(fftlen-1-k);
      }
      from = tmp;
    }
    
    /* Set up the FFT server */
    FFTServer<T,S> server (IPosition(1,fftsize));
    
    if (realtocomplex) {
      Vector<S> input(fftlen);
      for (k=0; k<fftlen; k++) {
	input(k) = conj(from(fftlen-1-k));
      }
      server.fft(dataFx,input,true);
    } else {
      Vector<S> input(fftsize);
      
      // fill the lower half of the input vector to the FFT
      for (k=0; k<fftlen; k++) {
	input(k) = from(k);
      }
      
      // fill the upper half of the input vector to the FFT
      if ((fftsize%2)==1) { // odd
	for (k=fftsize-halfsize; k<fftsize; k++) {
	  input(k) = conj(from(fftsize-k-1));
	}
      } else { // even
	for (int k=fftsize-halfsize; k<fftsize; k++) {
	  input(k) = conj(from(fftsize-k));
	}
      }
      
      // perform the invers fft
      server.fft(input,false);
      
      dataFx.resize(fftsize);
      dataFx = real(input);
    }
    
    // ... and return the result
    return dataFx;
  }
  
  // --------------------------------------------------------------------- FFT2Fx
  
  template <typename T, typename S>
  Vector<T> FFT2Fx (Vector<S>& fft,
		    int const &fftsize)
  {
    Vector<T> fx (fftsize);
    
    FFTServer<T,S> server(IPosition(1,fftsize),
			  casa::FFTEnums::REALTOCOMPLEX);
    
    server.fft(fx,fft);
    
    return fx;
  }
  
  // ============================================================================
  //
  //  Template initialization
  //
  // ============================================================================

  template Vector<Float> FFT2Fx (Vector<Complex>& from,
			       int const &fftsize,
			       bool const &invdir,
			       bool const &realtocomplex);
  
  template Vector<Double> FFT2Fx (Vector<DComplex>& from,
				  int const &fftsize,
				  bool const &invdir,
				  bool const &realtocomplex);
  
  template Vector<Float> FFT2Fx (Vector<Complex>& fft,
				 int const &fftsize);
  
  template Vector<Double> FFT2Fx (Vector<DComplex>& fft,
				  int const &fftsize);
  
} // Namespace LOPES -- end
