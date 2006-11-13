//#  PropagatedMeasurementError.cc: The propagated measurement errors.
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
//#  $Id: PropagatedMeasurementError.cc,v 1.7 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/PropagatedMeasurementError.h>
#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/PSF/ControlMatrix.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/Math.h>
#include <Sinfoni/Debug.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      //# An unnamed namespace is the C++ equivalent of the static keyword in C.
      namespace
      {
        // Compute the variance of the measurement error, which is defined as:
        // \f[
        //   \sigma_{n_w}^2 \approx \sigma_w^2 - \frac{\sigma_d^2}{N^2} 
        //                                     + \frac{1}{N}
        // \f]
        // where 
        // - \f$\sigma_w^2\f$ is the variance of the actual WFS measurements;
        // - \f$\sigma_d^2\f$ is the variance of the difference between the
        //   intra- and extra-focal flux on the WFS; and 
        // - \f$N\f$ is the sum of the intra- and extra-focal flux on the WFS.
        //
        // \note The method described by Veran e.a. gives a better accuracy,
        // but in practice it sometimes return negative values for the
        // variance. Hence you can choose whether you want to use Veran's
        // method, or do things the classical way. The default is to use
        // Veran's method.
        //
        // \todo Should we add the check for the constraint \f$\sigma_S \ll
        // N\f$?
        VectorReal measurementNoiseVariance(bool useVeransMethod = true)
        {
          //# Note: these constructors create references, not copies. Refer to
          //# the Blitz++ User's Guide for details.
          VectorReal var_w(diag(theInputData().curvatureCovariance()));
          VectorReal var_d(theInputData().diffCountsVariance());
          VectorReal N(theInputData().sumCountsMean());

          //# Allocate memory for the vector of variances.
          VectorReal var_nw(N.size());

          if (useVeransMethod) {
            var_nw = var_w + (1 - var_d / N) / N;
          } 
          else {
            var_nw = 1 / N;
          }
          return var_nw;
        }

      } //# namespace


      const MatrixReal& PropagatedMeasurementError::covariance()
      {
        if (isEmpty(itsCovariance)) {

          DBG_TRACE("Computing covariance matrix of the propagated "
                    "measurement errors ...");

          //# Allocate memory for the covariance matrix.
          itsCovariance.resize(theNrOfSubApertures, theNrOfSubApertures);

          //# Note: these constructors create references, not copies. Refer to
          //# the Blitz++ User's Guide for details.
          MatrixReal Cnwnw(diag(measurementNoiseVariance()));
          MatrixReal C(theControlMatrix());
          
          //# Calculate the matrix product C * Cnwnw * C'.  First we will
          //# calculate C * Cnwnw, next we will multiply the result with Ct.
          //# Refer to the Blitz++ User's Guide for details.
          using namespace tensor;
          MatrixReal tmp(sum(C(i,k) * Cnwnw(k,j), k));
          itsCovariance = sum(tmp(i,k) * C(j,k), k);

          //# Dump the covariance matrix if debugging is enabled.
          DBG_DUMP("PropagatedMeasurementError.out", itsCovariance);

        }
        return itsCovariance;
      }


      PropagatedMeasurementError& thePropagatedMeasurementError()
      {
        static PropagatedMeasurementError pme;
        return pme;
      }

    } //# namespace PSF

  } //# namespace SINFONI
  
} //# namespace ASTRON
