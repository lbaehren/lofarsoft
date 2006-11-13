//#  ModalCoordinates.h: modal coordinates of actual the mirror modes.
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
//#  $Id: ModalCoordinates.h,v 1.3 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_MODALCOORDINATES_H
#define SINFONI_PSF_MODALCOORDINATES_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This class represents the modal coordinates \f$\mathbf{a}\f$ or
      // coefficients of the actual mirror modes \f$\mathbf{M}\f$.
      // 
      // \note This class was created, only because we needed to have public
      // access to the variance of the modal coordinates, which is a quality
      // metric.
      class ModalCoordinates
      {
      public:
        // The variance of the modal coordinates can be estimated using the
        // following formula:
        // \f[
        //   \sigma^2_{a_i} = \sigma^2_{m_i} - \sigma^2_{r_i} 
        //                  - \sigma^2_{n_i} \int |H_n(g_i,f)|^2 df
        // \f]
        // where \f$\sigma^2_{m_i}\f$ is the variance of the modal commands to
        // the deformable mirror; \f$\sigma^2_{r_i}\f$ is the variance of the
        // remaining (or aliasing) error, originating from the high-order
        // components of the turbulent phase; \f$\sigma^2_{n_i}\f$ is the
        // variance of the propagated WFS measurements errors onto the mirror
        // modes; and \f$H_n(g_i,f)\f$ is the noise transfer function.
        //
        // \note We need \a d_r0, the estimate of the \f$D/r_0\f$, in order to
        // calculate the variance of the remaining (or aliasing) error
        // \f$\sigma^2_{r_i}\f$.
        //
        // \see modalCommandsCovariance(), RemainingError::covariance(),
        // PropagatedMeasurementError::covariance(), and noiseTransfer().
        static VectorReal variance(Real d_r0);
      };

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
