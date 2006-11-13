//#  ApertureToFriedRatio.h: This class represents the (estimate of) D/r_0.
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
//#  $Id: ApertureToFriedRatio.h,v 1.4 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_APERTURETOFRIEDRATIO_H
#define SINFONI_PSF_APERTURETOFRIEDRATIO_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the estimate of \f$D/r_0\f$, where \f$D\f$ is
      // the diameter of the telescope aperture, and \f$r_0\f$ is the Fried
      // parameter of the turbulence. The estimate can be calculated
      // iteratively, using:
      // - \f$\mathbf{C}_{mm}\f$, the covariance matrix of the modal commands
      //   to the deformable mirror;
      // - \f$\mathbf{C}_{rr}\f$, the covariance matrix of the remaining (or
      //   aliasing) error;
      // - \f$\mathbf{C}_{nn}\f$, the covariance matrix of propagation of the
      //   WFS measurement errors onto the mirror modes; and
      // - \f$H_n(g_i,f)\f$, the noise transfer function.
      //
      // \note This class acts like a Singleton class, but it is implemented a
      // bit different from the original design pattern. We use the global
      // function theApertureToFriedRatio() to return the estimate of
      // \f$D/r_0\f$. This function stores a static instance of
      // ApertureToFriedRatio, hence it will be constructed when
      // theApertureToFriedRatio() is called for the first time.
      //
      // \see modalCommandsCovariance(), RemainingError::covariance(),
      // PropagatedMeasurementError::covariance(), and noiseTransfer().
      class ApertureToFriedRatio
      {
      private:
        // The friend function is used to return the contents of the
        // estimate of \f$D/r_0\f$.
        friend Real theApertureToFriedRatio();
        
        //@{
        // Disallow copying and assignment. 
        ApertureToFriedRatio(const ApertureToFriedRatio&);
        ApertureToFriedRatio& operator=(const ApertureToFriedRatio&);
        //@}

        // Default constructor. When called, it triggers the calculation of
        // the estimate of \f$D/r_0\f$. The result of this calculation is
        // cached in the local variable \c itsEstimate.
        //
        // \note Construction is done by the friend function
        // theApertureToFriedRatio().
        ApertureToFriedRatio();

        // Compute the estimation of \f$D/r_0\f$. This is an iterative
        // process, that will normally converge rather quickly. An exception
        // is thrown when the estimation fails to converge within a predefined
        // number of iterations.
        void estimate();

        // The cached result of the estimation of \f$D/r_0\f$.
        Real itsEstimate;
      };

      // Return the estimate of \f$D/r_0\f$.
      Real theApertureToFriedRatio();

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
