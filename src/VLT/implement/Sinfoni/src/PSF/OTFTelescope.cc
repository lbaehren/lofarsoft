//#  OTFTelescope.cc: OTF of the telescope.
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
//#  $Id: OTFTelescope.cc,v 1.3 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/OTFTelescope.h>
#include <Sinfoni/FFT.h>
#include <Sinfoni/Debug.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      OTFTelescope::OTFTelescope()
      {
        autocorr();
      }

      void OTFTelescope::autocorr()
      {
        //# Create a reference to the matrix containing the pupil.
        MatrixInteger pupil(theVLTPupil().mask());

        //# The matrix \a itsMatrix containing the autocorrelation product
        //# should be twice the size of the matrix containing the pupil in
        //# order to avoid wrap-around effects.
        itsMatrix.resize(2*pupil.rows(), 2*pupil.cols());

        //# Create a temporary matrix for storing intermediate results.
        MatrixComplex tmp(itsMatrix.shape());

        //# Place the pupil in the center of the matrix \a tmp (assumming the
        //# size of \a pupil is even).
        Integer lo(pupil.rows()/2);
        Integer hi(lo+pupil.rows()-1);
        tmp(Range(lo,hi), Range(lo,hi)) = cast<Complex>(pupil);
        
        //# Calculate the autocorrelation function, using the Wiener-Khinchin
        //# theorem.
        tmp = fft2(tmp);     //# calculate the 2D Fourier transform
        tmp *= conj(tmp);    //# multiply by the complex conjugate
        tmp = ifft2(tmp);    //# calculate the inverse 2D Fourier transform
        tmp = fftshift(tmp); //# shift zero-frequency component to center
        //# Assign the real part to the matrix \a itsMatrix; the imaginary part
        //# should only contain numeric noise.
        itsMatrix = real(tmp);

        //# To avoid numerical instabilities when dividing by the
        //# autocorrelation, set any value smaller than \f$10^{-11}\f$ equal
        //# to 1.
        itsMatrix = where(itsMatrix < 1e-11, 1, itsMatrix);

        //# Normalize the matrix \a itsMatrix and return it.
        itsMatrix /= max(itsMatrix);

        DBG_DUMP("OTFTelescope.out", itsMatrix);
      }


      const MatrixReal& theOTFTelescope()
      {
        static OTFTelescope otft;
        return otft.itsMatrix;
      }

    } // namespace PSF

  } // namespace SINFONI
  
} // namespace ASTRON
