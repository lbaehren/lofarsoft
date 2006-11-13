//#  ApertureToFriedRatio.cc:  This class represents the (estimate of) D/r_0.
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
//#  $Id: ApertureToFriedRatio.cc,v 1.7 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/ApertureToFriedRatio.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/PSF/ModalCoordinates.h>
#include <Sinfoni/PSF/TransformedNollMatrix.h>
#include <Sinfoni/Math.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/Debug.h>
#include <cerrno>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      ApertureToFriedRatio::ApertureToFriedRatio() : itsEstimate(0.0)
      {
        //# Compute the estimation of \f$D/r_0\f$.
        estimate();

        //# Dump the result to file if debugging is enabled.
        DBG_DUMP("ApertureToFriedRatio.out", itsEstimate);
      }

      
      void ApertureToFriedRatio::estimate()
      {
        DBG_TRACE("Estimating Aperture diameter to Fried parameter ratio ...");

        //# The maximum number of iterations that will be done to calculate an
        //# estimate of \f$D/r_0\f$.
        const Integer maxIteration(100);

        //# The maximum absolute error that is allowed in the estimate of
        //# \f$D/r_0\f$.
        const Real maxError(1e-4);

        //# Initialize \a error to the maximum real value.
        Real error(numeric_limits<Real>::max());

        //# Calculate the variance theoretical mirror modes.
        VectorReal Kij(diag(theTransformedNollMatrix()));

        //# Start the iteration.
        for (Integer count=0; count < maxIteration; count++) {

//           DBG_TRACE("iteration #" << count << 
//                     ": itsEstimate = " << itsEstimate << 
//                     ", error = " << error);
          
          if (error < maxError) break;
          
          //# Get the variance of the mode coefficients in the turbulent
          //# phase.
          VectorReal var_a(ModalCoordinates::variance(itsEstimate));
          
          //# Replace any negative value in \a var_a with zero.
          var_a = where(var_a < 0, 0, var_a);

          //# Calculate an estimate of \f$D/r_0\f$ for each mirror mode.
          VectorReal d_r0(pow(var_a/Kij, Real(3.0/5.0)));
          if (errno) THROW (MathException, strerror(errno));

          //# Remove the piston and tip/tilt modes; take the mean of the
          //# remaining estimates as the new estimate for \f$D/r_0\f$.
          Real d_r0_new = mean(d_r0(Range(3,theNrOfUsedMirrorModes-1)));

          //# Calculate the error, which is defined as the absolute value of
          //# the difference between the new estimate \a d_r0_new and the
          //# current estimate \a itsEstimate.
          error = abs(d_r0_new - itsEstimate);

          //# Assign the new estimate
          itsEstimate = d_r0_new;

        }

        if (!finite(error) || error >= maxError)
          THROW (MathException, "Estimation of D/r_0 failed to converge after "
                 << maxIteration << " iterations");

      }


      Real theApertureToFriedRatio()
      {
        static ApertureToFriedRatio d_r0;
        return d_r0.itsEstimate;
      }

    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
