//#  NoiseTransfer.h: Global methods needed to calculate the noise transfer.
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
//#  $Id: NoiseTransfer.h,v 1.10 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_NOISETRANSFER_H
#define SINFONI_PSF_NOISETRANSFER_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // Compute the noise transfer, which is defined as: 
      // \f[
      //   \int |H_n(g_i,f)|^2 df
      // \f] 
      // where \f$H_n(g_i,f)\f$ is the so-called noise transfer function.
      //
      // \note We took a quick and dirty approach to calculate the integral
      // \f$ \int |H_n(g_i,f)|^2 df \f$, using a fixed step-size \c df. A
      // better approach would be, of course, to use an algorithm that uses
      // user-specified boundary conditions like maximum absolute error and
      // maximum relative error (e.g. an algorithm based on the Gauss-Kronrod
      // rule).
      Real noiseTransfer(Real gain);

      // Compute the noise transfer function for a vector of gains.
      VectorReal noiseTransfer(const VectorReal& gain);

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
