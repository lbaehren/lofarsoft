//#  OTFMirrorCompContrib.h: Contribution of mirror component to OTF.
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
//#  $Id: OTFMirrorCompContrib.h,v 1.6 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_OTFMIRRORCOMPCONTRIB_H
#define SINFONI_PSF_OTFMIRRORCOMPCONTRIB_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the contribution of the mirror component of the
      // residual phase to the long exposure optical transfer function,
      // denoted by \f$\mathbf{B}_{\epsilon\parallel}\f$. It can be computed
      // using the formula:
      // \f[
      //   \mathbf{B}_{\epsilon\parallel}(\boldsymbol{\rho}/\lambda) = 
      //   \exp(-\frac{1}{2} 
      //         \mathbf{D}_{\phi_{\epsilon\parallel}}(\boldsymbol{\rho}))
      // \f]
      // where \f$\mathbf{D}_{\phi_{\epsilon\parallel}}(\boldsymbol{\rho})\f$
      // is the structure function for the mirror component of the residual
      // phase \f$\phi_{\epsilon\parallel}\f$.
      //
      // This structure function can be calculated using the formula:
      // \f[
      //   \mathbf{D}_{\phi_{\epsilon\parallel}}(\boldsymbol{\rho}) = 
      //     \sum_{i,j} <\epsilon_i\epsilon_j> U_{ij}(\boldsymbol{\rho})
      // \f]
      // where
      // - \f$<\epsilon_i\epsilon_j>\f$ is the element \f$(i,j)\f$ of the
      //   covariance matrix of modal coordinates of the residual phase
      //   \f$C_{\epsilon\epsilon}\f$; and
      // - \f$U_{ij}(\boldsymbol{\rho})\f$ is a function of the mirror modes
      //   that is precalculated and will be loaded from file.
      class OTFMirrorCompContrib
      {
      private:
        // The friend function is used to return the contribution of the
        // mirror component of the residual phase to the long exposure OTF.
        friend MatrixReal theOTFMirrorCompContrib();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        OTFMirrorCompContrib();
        OTFMirrorCompContrib(const OTFMirrorCompContrib&);
        OTFMirrorCompContrib& operator=(const OTFMirrorCompContrib&);
        //@}

        // This is the only implemented constructor. It creates a square
        // matrix of size \a size. The structure function must be calculated,
        // using the precalculated functions of the mirror modes. The result
        // will be cached in the data member \c itsStructFunc.
        OTFMirrorCompContrib(Integer size);

        // Calculate the structure function for the mirror component of the
        // residual phase and store the result in the data member \c
        // itsStructFunc.
        void structFunc();

        // Matrix containing the structure function for the mirror component
        // of the residual phase.
        MatrixReal itsStructFunc;
      };
      
      // Return the contribution of the mirror component of the residual phase
      // to the long exposure OTF.
      MatrixReal theOTFMirrorCompContrib(Real d_r0);

    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
