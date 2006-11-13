//#  ModalCoordResidualPhase.cc: Modal coordinates of the residual phase.
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
//#  $Id: ModalCoordResidualPhase.cc,v 1.7 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/ModalCoordResidualPhase.h>
#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/PSF/ControlMatrix.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/PSF/PropagatedMeasurementError.h>
#include <Sinfoni/PSF/RemainingError.h>
#include <Sinfoni/PSF/ApertureToFriedRatio.h>
#include <Sinfoni/Math.h>
#include <Sinfoni/Debug.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      MatrixReal ModalCoordResidualPhase::covarianceEstimate()
      {
        //# Note: these constructors create references, not copies. 
        //# Refer to the Blitz++ User's Guide for details.
        MatrixReal Cww(theInputData().curvatureCovariance());
        MatrixReal C(theControlMatrix());

        //# Calculate the matrix product C * Cww * C'. First we will
        //# calculate C * Cww, next we will multiply the result with C'.
        //# Refer to the Blitz++ User's Guide for details.
        using namespace tensor;
        MatrixReal tmp(sum(C(i,k) * Cww(k,j), k));
        MatrixReal Ceses(sum(tmp(i,k) * C(j,k), k));

        //# Return the result
        return Ceses;
      }


      const MatrixReal& ModalCoordResidualPhase::covariance()
      {
        if (isEmpty(itsCovariance)) {

          DBG_TRACE("Computing covariance matrix of the modal coordinates "
                    "of the residual phase ...");

          //# Allocate memory for the covariance matrix.
          itsCovariance.resize(theNrOfSubApertures, theNrOfSubApertures);

          //# First calculate the covariance of the real-time computer (RTC)
          //# estimate of the modal coordinates of the residual phase.
          itsCovariance = covarianceEstimate();

          //# Next subtract the covariance of the propagated measurement error.
          itsCovariance -= thePropagatedMeasurementError().covariance();

          Real d_r0(theApertureToFriedRatio());
          MatrixReal Crr(theRemainingError().covariance(d_r0));

          //# Finally add the covariance of the remaining (or aliasing) error.
          itsCovariance += 
            theRemainingError().covariance(theApertureToFriedRatio());

          //# Dump the covariance matrix if debugging is enabled.
          DBG_DUMP("ModalCoordResidualPhase.out", itsCovariance);

        }
        return itsCovariance;
      }


      ModalCoordResidualPhase& theModalCoordResidualPhase()
      {
        static ModalCoordResidualPhase mcrp;
        return mcrp;
      }


    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
