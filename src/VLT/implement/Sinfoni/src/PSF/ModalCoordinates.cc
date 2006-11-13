//#  ModalCoordinates.cc: modal coordinates of actual the mirror modes.
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
//#  $Id: ModalCoordinates.cc,v 1.4 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/ModalCoordinates.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/PSF/VoltageToZernike.h>
#include <Sinfoni/PSF/NoiseTransfer.h>
#include <Sinfoni/PSF/PropagatedMeasurementError.h>
#include <Sinfoni/PSF/RemainingError.h>

using namespace blitz;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // Place the following local methods in an unnamed namespace, which is
      // the C++ way of defining static methods.
      namespace 
      {
        // Return the covariance matrix of the modal commands applied to the
        // deformable mirrors. The InputData::mirrorCovariance() returns the
        // covariance matrix of the voltages to applied the deformable mirrors
        // on the so-called voltage basis. Hence, we must transform this
        // matrix to the Zernike basis, using theVoltageToZernike()
        // transformation matrix.
        MatrixReal modalCommandsCovariance()
        {
          //# Note: these constructors create references, not copies. 
          //# Refer to the Blitz++ User's Guide for details.
          MatrixReal mirrorCov(theInputData().mirrorCovariance());
          MatrixReal cst(2*theVoltageToZernike());

          //# Calculate the matrix product cst * mirrorCov * cst'. First we
          //# will calculate cst * mirrorCov, next we will multiply the result
          //# with cst'. Refer to the Blitz++ User's Guide for details.
          using namespace tensor;
          MatrixReal tmp(sum(cst(i,k) * mirrorCov(k,j), k));
          MatrixReal result(sum(tmp(i,k) * cst(j,k), k));

          //# Return the result
          return result;
        }

      } //# namespace 


      VectorReal ModalCoordinates::variance(Real d_r0)
      {
        //# Declare the following four variables static, because we only need
        //# to calculate them once and can retain their values between calls.

        //# The integrated noise transfer function.
        static VectorReal 
          Hn_int(noiseTransfer(theInputData().gainMean()));

        //# The variance of the propagated measurement errors.
        static VectorReal 
          var_n(diag(thePropagatedMeasurementError().covariance()));

        //# The variance of the modal commands.
        static VectorReal 
          var_m(diag(modalCommandsCovariance()));

        //# The variance of the remaining (or aliasing) errors.
        VectorReal var_r(diag(theRemainingError().covariance(d_r0)));

        //# The variance of the mode coefficients in the turbulent phase.
        VectorReal var_a(var_m - var_r - var_n * Hn_int);

        //# Return \a var_a
        return var_a;
      }

    } // namespace PSF

  } // namespace SINFONI
  
} // namespace ASTRON
