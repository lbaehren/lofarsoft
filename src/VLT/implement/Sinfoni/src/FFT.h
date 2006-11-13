//#  FFT.h: FFT methods defined on Blitz arrays.
//#
//#  Copyright (C) 2002-2006
//#  ASTRON (Netherlands Foundation for Research in Astronomy)
//#  P.O.Box 2, 7990 AA Dwingeloo, The Netherlands, seg@astron.nl
//#
//#  This program is free software; you can redistribute it and/or modify
//#  it under the terms of the GNU General Public License as published by
//#  the Free Software Foundation; either version 2 of the License, or
//#  (at your option) any later version.
//#
//#  This program is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY; without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//#  GNU General Public License for more details.
//#
//#  You should have received a copy of the GNU General Public License
//#  along with this program; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//#
//#  $Id: FFT.h,v 1.6 2006/05/19 16:03:23 loose Exp $

#ifndef SINFONI_FFT_H
#define SINFONI_FFT_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    //# The method declarations between "#if 0" and "#endif" are not
    //# implemented yet. There are two ways to do it:
    //#
    //# 1. The quick and dirty way: simply create a temporary complex vector
    //#    or matrix to hold the input argument and forward the call to one
    //#    of the complex-to-complex fft routines.
    //# 2. The elegant way, taking advantage of the fact that the FFT of real
    //#    input data has some symmetrical properties. This latter approach
    //#    will also require new wrappers for some other FFTW functions.
    //#
    //# Note that the 1D FFT routines for matrices have also not been
    //# implemented yet. Use Matlab as a reference when implementing them.

    VectorComplex fft(const VectorComplex& x, int size = 0);
    VectorComplex ifft(const VectorComplex& x, int size = 0);

    MatrixComplex fft2(const MatrixComplex& x, int rows = 0, int cols = 0);
    MatrixComplex ifft2(const MatrixComplex& x, int rows = 0, int cols = 0);

#if 0
    VectorComplex fft(const VectorReal& x, int size = 0);
    VectorComplex ifft(const VectorReal& x, int size = 0);
    VectorComplex fft(const VectorInteger& x, int size = 0);
    VectorComplex ifft(const VectorInteger& x, int size = 0);

    MatrixComplex fft(const MatrixComplex& x, int rows = 0, int cols = 0);
    MatrixComplex ifft(const MatrixComplex& x, int rows = 0, int cols = 0);
    MatrixComplex fft(const MatrixReal& x, int rows = 0, int cols = 0);
    MatrixComplex ifft(const MatrixReal& x, int rows = 0, int cols = 0);
    MatrixComplex fft(const MatrixInteger& x, int rows = 0, int cols = 0);
    MatrixComplex ifft(const MatrixInteger& x, int rows = 0, int cols = 0);

    MatrixComplex fft2(const MatrixReal& x, int rows = 0, int cols = 0);
    MatrixComplex ifft2(const MatrixReal& x, int rows = 0, int cols = 0);
    MatrixComplex fft2(const MatrixInteger& x, int rows = 0, int cols = 0);
    MatrixComplex ifft2(const MatrixInteger& x, int rows = 0, int cols = 0);
#endif

    VectorComplex fftshift(const VectorComplex& x);
    VectorReal    fftshift(const VectorReal& x);
    VectorInteger fftshift(const VectorInteger& x);

    MatrixComplex fftshift(const MatrixComplex& x, int dim = -1);
    MatrixReal    fftshift(const MatrixReal& x,    int dim = -1);
    MatrixInteger fftshift(const MatrixInteger& x, int dim = -1);

    VectorComplex ifftshift(const VectorComplex& x);
    VectorReal    ifftshift(const VectorReal& x);
    VectorInteger ifftshift(const VectorInteger& x);

    MatrixComplex ifftshift(const MatrixComplex& x, int dim = -1);
    MatrixReal    ifftshift(const MatrixReal& x,    int dim = -1);
    MatrixInteger ifftshift(const MatrixInteger& x, int dim = -1);


  } // namespace SINFONI

} // namespace ASTRON

#endif
