//#  ModalCoordResidualPhase.h: Modal coordinates of the residual phase.
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
//#  $Id: ModalCoordResidualPhase.h,v 1.5 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_MODALCOORDRESIDUALPHASE_H
#define SINFONI_PSF_MODALCOORDRESIDUALPHASE_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the modal coordinates of the residual phase,
      // denoted by \f$\boldsymbol{\epsilon}\f$. The covariance matrix of
      // \f$\boldsymbol{\epsilon}\f$, \f$\mathbf{C}_{\epsilon\epsilon}\f$, can
      // be estimated from \f$\mathbf{C}_{ww}\f$, the covariance matrix of the
      // actual WFS curvature measurements.
      //
      // \note This class acts like a Singleton class, but it is implemented a
      // bit different from the original design pattern. We use the global
      // function theModalCoordResidualPhase() to return a reference to
      // an instance of ModalCoordResidualPhase. This function stores a
      // static instance of ModalCoordResidualPhase, hence it will be
      // constructed when theModalCoordResidualPhase() is called for the
      // first time.
      class ModalCoordResidualPhase
      {
      public:
        // Return the covariance matrix of the modal coordinates of the
        // residual phase \f$\mathbf{C}_{\epsilon\epsilon}\f$, which can be
        // approximated using the following formula:
        // \f[
        //   \mathbf{C}_{\epsilon\epsilon} \approx 
        //     \mathbf{C}_{\hat{\epsilon}\hat{\epsilon}} 
        //      - \mathbf{C}_{nn} + \mathbf{C}_{rr}
        // \f]
        // where 
        // - \f$\mathbf{C}_{\hat{\epsilon}\hat{\epsilon}}\f$ is the covariance
        //   matrix of the real-time computer (RTC) estimate of the modal 
        //   coordinates of the residual phase, which can be calculated using
        //   the formula:
        //   \f[
        //     \mathbf{C}_{\hat{\epsilon}\hat{\epsilon}} = 
        //       \mathbf{D}^+ \mathbf{C}_{ww} (\mathbf{D}^+)^T
        //   \f]
        //
        // - \f$\mathbf{C}_{nn}\f$ is the covariance matrix of the propagation
        //   of the WFS measurement errors onto the mirror modes; and
        // - \f$\mathbf{C}_{rr}\f$ is the covariance matrix of the remaining
        //   (or aliasing) error.
        //
        // \note The covariance matrix will be calculated once, the result
        // will be cached in the local variable \c itsCovariance.
        // 
        // \see PropagatedMeasurementError, RemainingError.
        const MatrixReal& covariance();

        // Calculate the covariance matrix of the real-time computer (RTC)
        // estimate of the modal coordinates of the residual phase. It can be
        // calculated using the following formula:
        // \f[
        //   \mathbf{C}_{\hat{\epsilon}\hat{\epsilon}} = 
        //     \mathbf{D}^+ \mathbf{C}_{ww} (\mathbf{D}^+)^T
        // \f]
        // where \f$\mathbf{D}^+\f$ is the so-called control matrix, and
        // \f$\mathbf{C}_{ww}\f$ is the covariance matrix of the actual WFS
        // curvature measurements.
        //
        // \see ControlMatrix, InputData::curvatureCovariance().
        MatrixReal ModalCoordResidualPhase::covarianceEstimate();

      private:
        // The friend function is used to return an instance of the
        // ModalCoordResidualPhase class.
        friend ModalCoordResidualPhase& theModalCoordResidualPhase();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        ModalCoordResidualPhase() {}
        ModalCoordResidualPhase(const ModalCoordResidualPhase&);
        ModalCoordResidualPhase& operator=(const ModalCoordResidualPhase&);
        //@}

        // The covariance matrix of the modal coordinates of the residual
        // phase.
        MatrixReal itsCovariance;
     };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
