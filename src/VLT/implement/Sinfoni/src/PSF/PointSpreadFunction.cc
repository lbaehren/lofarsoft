//#  PointSpreadFunction.cc: class representing the Point Spread Function.
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
//#  $Id: PointSpreadFunction.cc,v 1.8 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/PointSpreadFunction.h>
#include <Sinfoni/PSF/ApertureToFriedRatio.h>
#include <Sinfoni/PSF/OTFHighOrderPhaseContrib.h>
#include <Sinfoni/PSF/OTFMirrorCompContrib.h>
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

      PointSpreadFunction::PointSpreadFunction()
      {
        normalizedPSF();
      }


      void PointSpreadFunction::normalizedPSF()
      {
        DBG_TRACE("Computing the normalized point spread function (PSF)");

        //# Calculate the OTF
        MatrixReal otf(theOTFMirrorCompContrib());
        otf *= theOTFHighOrderPhaseContrib(theApertureToFriedRatio());
        otf *= theOTFTelescope();
        
        //# Resize itsMatrix to the next power of two of the size of \a otf.
        int sz = max(otf.rows(), otf.cols());
        sz = static_cast<int>(pow(2, ceil(log2(sz+1))));
        itsMatrix.resize(sz, sz);

        //# We must first create a temporary complex matrix for doing the FFT,
        //# because fft2(MatrixReal&, int, int) has not been implemented yet.
        MatrixComplex tmp(cast<Complex>(otf));

        //# Calculate the 2-D Fourier transform, shift the zero frequency
        //# point to the center and take the absolute value.
        itsMatrix = abs(fftshift(fft2(tmp,sz,sz)));

        //# Normalize \a itsMatrix
        itsMatrix /= max(itsMatrix);

        //# Dump the normalized PSF if debugging is enabled.
        DBG_DUMP("PSFnorm.out", itsMatrix);
      }


      const MatrixReal& thePointSpreadFunction()
      {
        static PointSpreadFunction psf;
        return psf.itsMatrix;
      }


    } // namespace PSF

  } // namespace SINFONI
  
} // namespace ASTRON
