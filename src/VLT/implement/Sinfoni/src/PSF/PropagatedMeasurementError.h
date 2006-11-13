//#  PropagatedMeasurementError.h: The propagated measurement errors.
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
//#  $Id: PropagatedMeasurementError.h,v 1.5 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_PROPAGATEDMEASUREMENTERROR_H
#define SINFONI_PSF_PROPAGATEDMEASUREMENTERROR_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the propagation of the WFS measurement errors
      // \f$\mathbf{n}_w\f$ onto the mirror modes, denoted by
      // \f$\mathbf{n}\f$.
      //
      // \note This class acts like a Singleton class, but it is implemented a
      // bit different from the original design pattern. We use the global
      // function thePropagatedMeasurementError() to return a reference to an
      // instance of PropagatedMeasurementError. This function stores a static
      // instance of PropagatedMeasurementError, hence it will be constructed
      // when thePropagatedMeasurementError() is called for the first time.
      class PropagatedMeasurementError
      {
      public:
        // Return the covariance matrix of the propagated measurement error
        // \f$\mathbf{C}_{nn}\f$, which can be computed using the following
        // formula:
        // \f[
        // \mathbf{C}_{nn} = \mathbf{D}^+ \mathbf{C}_{n_wn_w} (\mathbf{D}^+)^T
        // \f]
        // where \f$\mathbf{D}^+\f$ is the generalized inverse of the modal
        // interaction matrix, also known as the control matrix; and
        // \f$\mathbf{C}_{n_wn_w}\f$ is the covariance matrix of
        // \f$\mathbf{n}_w\f$, the measurement errors in the WFS measurements.
        //
        // \see ControlMatrix
        //
        // \note The covariance matrix will be calculated once, the result
        // will be cached in the local variable \c itsCovariance.
        const MatrixReal& covariance();

      private:
        // The friend function is used to return an instance of the
        // PropagatedMeasurementError class.
        friend PropagatedMeasurementError& thePropagatedMeasurementError();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        PropagatedMeasurementError() {}
        PropagatedMeasurementError(const PropagatedMeasurementError&);
        PropagatedMeasurementError& operator=(const PropagatedMeasurementError&);
        //@}

        // The covariance matrix of the propagated measurement error.
        MatrixReal itsCovariance;
      };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
