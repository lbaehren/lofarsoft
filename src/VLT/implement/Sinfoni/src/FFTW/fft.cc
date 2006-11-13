//#  fft.cc: implementation of FFT methods defined on Blitz arrays.
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
//#  $Id: fft.cc,v 1.8 2006/05/19 16:03:23 loose Exp $

#include <fftw3.h>
#include <Sinfoni/Assert.h>

//# Define concatenation macro
#define CONCAT(x,y) x ## y

//# Define a macro that generates the correct data types and function names,
//# depending on the type definition of Real.
#if defined(USE_SINGLE_PRECISION)
#define FFTW(x) CONCAT(fftwf_, x)
#else
#define FFTW(x) CONCAT(fftw_, x)
#endif

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    //# Put these methods in a unnamed namespace, which is the C++ equivalent
    //# of declaring the method static in C.
    namespace 
    {

      VectorComplex fft1d(const VectorComplex& x, int size, int sign)
      {
        //# Just to make sure no-one is supplying the wrong sign.
        Assert(sign == FFTW_FORWARD || sign == FFTW_BACKWARD);

#if defined(SINFONI_DEBUG)
        VectorComplex old_x(x.copy());
#endif

        //# Local vector that contains the values of the input vector \a x.
        //# \note No copy is being made, \a src is just a reference to \a x.
        //# See the Blitz++ User Guide for details.
        VectorComplex src(x);

        //# If \a size is larger than the size of the input vector \a x, we
        //# must create a copy of the input vector \a x and pad it with zeros.
        if (size > x.size()) {
          src.resize(size);
          src = 0;
          src(Range(0, x.size()-1)) = x;
        }
        
        //# This vector will contain the Fourier Transform of \a x.
        VectorComplex dest(size);

        //# Cast the input and output parameters to FFTW types. This can be
        //# done safely using a reinterpret_cast<>. For details, refer to the
        //# FFTW User Manual, section 4.1.1.
        FFTW(complex)* in = reinterpret_cast<FFTW(complex)*>(src.data());
        FFTW(complex)* out = reinterpret_cast<FFTW(complex)*>(dest.data());

        //# Create a plan.
        FFTW(plan) p = 
          FFTW(plan_dft_1d)(size, in, out, sign, FFTW_ESTIMATE);

        //# Calculate the Fourier Transform
        FFTW(execute)(p);

        //# Destroy the plan
        FFTW(destroy_plan)(p);

#if defined(SINFONI_DEBUG)
        AssertStr(all(x == old_x), "`const VectorComplex& x' has changed" );
#endif

        //# Return the Fourier Transform.
        return dest;
      }


      MatrixComplex fft2d(const MatrixComplex& x, int rows, int cols, int sign)
      {
        //# Just to make sure no-one is supplying the wrong signection.
        Assert(sign == FFTW_FORWARD || sign == FFTW_BACKWARD);

#if defined(SINFONI_DEBUG)
        MatrixComplex old_x(x.copy());
#endif

        //# Local matrix that contains the values of the input matrix \a x.
        //# \note No copy is being made, \a src is just a reference to \a x.
        //# See the Blitz++ User Guide for details.
        MatrixComplex src(x);

        //# If either \a rows > x.rows() or \a cols > x.cols(), we must create
        //# a copy of the input matrix \a x and pad it with zeros.
        if (rows > x.rows() || cols > x.cols()) {
          src.resize(rows, cols);
          src = 0;
          src(Range(0, x.rows()-1), Range(0, x.cols()-1)) = x;
        }

        //# This matrix will contain the Fourier Transform of \a x.
        MatrixComplex dest(rows, cols);

        //# Cast the input and output parameters to FFTW types. This can be
        //# done safely using a reinterpret_cast<>. For details, refer to the
        //# FFTW User Manual, section 4.1.1.
        FFTW(complex)* in = reinterpret_cast<FFTW(complex)*>(src.data());
        FFTW(complex)* out = reinterpret_cast<FFTW(complex)*>(dest.data());
        
        //# Create a plan.
        FFTW(plan) p = 
          FFTW(plan_dft_2d)(rows, cols, in, out, sign, FFTW_ESTIMATE);
        
        //# Calculate the Fourier Transform
        FFTW(execute)(p);
        
        //# Destroy the plan
        FFTW(destroy_plan)(p);
        
#if defined(SINFONI_DEBUG)
        AssertStr(all(x == old_x), "`const MatrixComplex& x' has changed" );
#endif
        
        //# Return the Fourier Transform.
        return dest;
      }

    } // namespace


    VectorComplex fft(const VectorComplex& x, int size)
    {
      if (size == 0) size = x.size();
      return fft1d(x, size, FFTW_BACKWARD);
    }


    VectorComplex ifft(const VectorComplex& x, int size)
    {
      if (size == 0) size = x.size();
      VectorComplex ft = fft1d(x, size, FFTW_FORWARD);
      ft /= size;
      return ft;
    }


    MatrixComplex fft2(const MatrixComplex& x, int rows, int cols)
    {
      if (rows == 0 || cols == 0) rows = x.rows(), cols = x.cols();
      return fft2d(x, rows, cols, FFTW_FORWARD);
    }


    MatrixComplex ifft2(const MatrixComplex& x, int rows, int cols)
    {
      if (rows == 0 || cols == 0) rows = x.rows(), cols = x.cols();
      MatrixComplex ft = fft2d(x, rows, cols, FFTW_BACKWARD);
      ft /= (rows*cols);
      return ft;
    }


    VectorComplex fftshift(const VectorComplex& x)
    {
      VectorComplex xx(x.shape());
      int rows = x.rows();
      for (int r = 0; r < rows; r++) xx((r+rows/2)%rows) = x(r);
      return xx;
    }


    VectorReal fftshift(const VectorReal& x)
    {
      VectorReal xx(x.shape());
      int rows = x.rows();
      for (int r = 0; r < rows; r++) xx((r+rows/2)%rows) = x(r);
      return xx;
    }


    VectorInteger fftshift(const VectorInteger& x)
    {
      VectorInteger xx(x.shape());
      int rows = x.rows();
      for (int r = 0; r < rows; r++) xx((r+rows/2)%rows) = x(r);
      return xx;
    }



    MatrixComplex fftshift(const MatrixComplex& x, int dim)
    {
      AssertStr(-1 <= dim && dim <= 1, "Illegal dimension: dim = " << dim);
      MatrixComplex xx(x.shape());
      int rows = x.rows();
      int cols = x.cols();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          switch(dim) {
          case -1:
            xx((r+rows/2)%rows, (c+cols/2)%cols) = x(r,c);
            break;
          case 0:
            xx((r+rows/2)%rows, c) = x(r,c);
            break;
          case 1:
            xx(r, (c+cols/2)%cols) = x(r,c);
            break;
          }
        }
      }
      return xx;
    }


    MatrixReal fftshift(const MatrixReal& x, int dim)
    {
      AssertStr(-1 <= dim && dim <= 1, "Illegal dimension: dim = " << dim);
      MatrixReal xx(x.shape());
      int rows = x.rows();
      int cols = x.cols();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          switch(dim) {
          case -1:
            xx((r+rows/2)%rows, (c+cols/2)%cols) = x(r,c);
            break;
          case 0:
            xx((r+rows/2)%rows, c) = x(r,c);
            break;
          case 1:
            xx(r, (c+cols/2)%cols) = x(r,c);
            break;
          }
        }
      }
      return xx;
    }


    MatrixInteger fftshift(const MatrixInteger& x, int dim)
    {
      AssertStr(-1 <= dim && dim <= 1, "Illegal dimension: dim = " << dim);
      MatrixInteger xx(x.shape());
      int rows = x.rows();
      int cols = x.cols();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          switch(dim) {
          case -1:
            xx((r+rows/2)%rows, (c+cols/2)%cols) = x(r,c);
            break;
          case 0:
            xx((r+rows/2)%rows, c) = x(r,c);
            break;
          case 1:
            xx(r, (c+cols/2)%cols) = x(r,c);
            break;
          }
        }
      }
      return xx;
    }


    VectorComplex ifftshift(const VectorComplex& x)
    {
      VectorComplex xx(x.shape());
      int rows = x.rows();
      for (int r=0; r<rows; r++) xx(r) = x((r+rows/2)%rows);
      return xx;
    }


    VectorReal ifftshift(const VectorReal& x)
    {
      VectorReal xx(x.shape());
      int rows = x.rows();
      for (int r=0; r<rows; r++) xx(r) = x((r+rows/2)%rows);
      return xx;
    }


    VectorInteger ifftshift(const VectorInteger& x)
    {
      VectorInteger xx(x.shape());
      int rows = x.rows();
      for (int r=0; r<rows; r++) xx(r) = x((r+rows/2)%rows);
      return xx;
    }



    MatrixComplex ifftshift(const MatrixComplex& x, int dim)
    {
      AssertStr(-1 <= dim && dim <= 1, "Illegal dimension: dim = " << dim);
      MatrixComplex xx(x.shape());
      int rows = x.rows();
      int cols = x.cols();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          switch(dim) {
          case -1:
            xx(r,c) = x((r+rows/2)%rows, (c+cols/2)%cols);
            break;
          case 0:
            xx(r,c) = x((r+rows/2)%rows, c);
            break;
          case 1:
            xx(r,c) = x(r, (c+cols/2)%cols);
            break;
          }
        }
      }
      return xx;
    }


    MatrixReal ifftshift(const MatrixReal& x, int dim)
    {
      AssertStr(-1 <= dim && dim <= 1, "Illegal dimension: dim = " << dim);
      MatrixReal xx(x.shape());
      int rows = x.rows();
      int cols = x.cols();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          switch(dim) {
          case -1:
            xx(r,c) = x((r+rows/2)%rows, (c+cols/2)%cols);
            break;
          case 0:
            xx(r,c) = x((r+rows/2)%rows, c);
            break;
          case 1:
            xx(r,c) = x(r, (c+cols/2)%cols);
            break;
          }
        }
      }
      return xx;
    }


    MatrixInteger ifftshift(const MatrixInteger& x, int dim)
    {
      AssertStr(-1 <= dim && dim <= 1, "Illegal dimension: dim = " << dim);
      MatrixInteger xx(x.shape());
      int rows = x.rows();
      int cols = x.cols();
      for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
          switch(dim) {
          case -1:
            xx(r,c) = x((r+rows/2)%rows, (c+cols/2)%cols);
            break;
          case 0:
            xx(r,c) = x((r+rows/2)%rows, c);
            break;
          case 1:
            xx(r,c) = x(r, (c+cols/2)%cols);
            break;
          }
        }
      }
      return xx;
    }


  } // namespace SINFONI
  
} // namespace ASTRON
