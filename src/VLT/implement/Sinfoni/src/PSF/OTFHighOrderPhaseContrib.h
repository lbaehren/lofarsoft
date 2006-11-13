//#  OTFHighOrderPhaseContrib.h: Contribution of high-order phase to OTF.
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
//#  $Id: OTFHighOrderPhaseContrib.h,v 1.4 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_OTFHIGHORDERPHASECONTRIB_H
#define SINFONI_PSF_OTFHIGHORDERPHASECONTRIB_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the contribution of the high-order phase to the
      // long exposure optical transfer function, denoted by
      // \f$\mathbf{B}_{\perp}\f$. It can be computed using the formula:
      // \f[
      //   \mathbf{B}_{\perp}(\boldsymbol{\rho}/\lambda) = 
      //     \exp(-\frac{1}{2} \mathbf{D}_{\phi_\perp}(\boldsymbol{\rho}))
      // \f]
      // where \f$\mathbf{D}_{\phi_\perp}(\boldsymbol{\rho})\f$ is the
      // structure function for the high-order component of the residual phase
      // \f$\phi_\perp\f$. This function is precalculated for \f$D/r_0=1\f$;
      // it must be calculated for other values of \f$D/r_0\f$ using the
      // formula:
      // \f[
      //   \mathbf{D}_{\phi_\perp}(\boldsymbol{\rho}) = 
      //     (\mathbf{D}_{\phi_\perp}(\boldsymbol{\rho}))_{D/r_0=1} 
      //       (\frac{D}{r_0})^{5/3}
      // \f]
      class OTFHighOrderPhaseContrib
      {
      private:
        // The friend function is used to return the contribution of the high
        // order phase to the long exposure OTF for the \f$D/r_0\f$ ratio \a
        // d_r0.
        friend MatrixReal theOTFHighOrderPhaseContrib(Real d_r0);

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        OTFHighOrderPhaseContrib();
        OTFHighOrderPhaseContrib(const OTFHighOrderPhaseContrib&);
        OTFHighOrderPhaseContrib& operator=(const OTFHighOrderPhaseContrib&);
        //@}

        // This is the only implemented constructor. It creates a square
        // matrix of size \a size. The data are read from the file
        // DataFile::structFuncHighOrderPhase and cached in the data member
        // \c itsStructFunc.
        OTFHighOrderPhaseContrib(Integer size);

        // Matrix containing the structure function for the high-order
        // component of the residual phase for \f$D/r_0=1\f$.
        MatrixReal itsStructFunc;
      };

      // Return the contribution of the high order phase to the long exposure
      // OTF for the \f$D/r_0\f$ ratio \a d_r0.
      MatrixReal theOTFHighOrderPhaseContrib(Real d_r0);

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
