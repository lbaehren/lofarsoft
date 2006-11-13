//#  PointSpreadFunction.h: Class representing the Point Spread Function.
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
//#  $Id: PointSpreadFunction.h,v 1.5 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_POINTSPREADFUNCTION_H
#define SINFONI_PSF_POINTSPREADFUNCTION_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      // This class represents the long exposure point spread function,
      // denoted by \f$\mathit{PSF}(\mathbf{u})\f$. It can be computed using
      // the formula:
      // \f[
      //   \mathit{PSF}(\mathbf{u}) = 
      //     \EuScript{F}\{\mathbf{B}(\boldsymbol{\rho}/\lambda)\}
      // \f]
      // where \f$\mathbf{B}(\boldsymbol{\rho}/\lambda)\f$ is the long
      // exposure optical transfer function (OTF), and \f$\EuScript{F}\{\}\f$
      // means taking the Fourier transform.
      // 
      // The OTF \f$\mathbf{B}(\boldsymbol{\rho}/\lambda)\f$ can be expressed
      // as the \e element-wise product of three contributions:
      // \f[
      //   \mathbf{B}_{ij} = 
      //      (\mathbf{B}_{\epsilon\parallel})_{ij} \,
      //      (\mathbf{B}_{\perp})_{ij} \,
      //      (\mathbf{B}_s)_{ij}
      // \f]
      // where \f$\mathbf{B}_{\epsilon\parallel}\f$ is the contribution of the
      // mirror component of the residual phase to the long exposure OTF;
      // \f$\mathbf{B}_{\perp}\f$ is the contribution of the high order phase
      // to the long exposure OTF; and \f$\mathbf{B}_s\f$ is the static OTF
      // (i.e. the OTF in the absence of turbulence and all the deformable
      // mirrors ``flat'').
      // 
      // \note Because the static OTF \f$\mathbf{B}_s\f$ is unknown, we will
      // approximate it using \f$\mathbf{B}_{tel}\f$, the OTF of the telescope
      // in the absence of turbulence.
      //
      // \see OTFMirrorCompContrib, OTFHighOrderPhaseContrib, OTFTelescope
      class PointSpreadFunction
      {
      private:
        // The friend function is used to return the normalized squared
        // absolute value of the point spread function.
        friend const MatrixReal& thePointSpreadFunction();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        PointSpreadFunction();
        PointSpreadFunction(const PointSpreadFunction&);
        PointSpreadFunction& operator=(const PointSpreadFunction&);
        //@}

        // Calculate the normalized squared absolute value of the long
        // exposure point spread function (PSF), which is defined as
        // \f[
        //   \mathit{PSF}_{\mathrm{norm}} = 
        //     \frac{|\mathit{PSF}(\mathbf{u})|^2}
        //          {\max(|\mathit{PSF}(\mathbf{u})|^2)}
        // \f]
        // and cache the result in \c itsMatrix.
        void normalizedPSF();

        // Matrix containing the normalized squared absolute value of the long
        // exposure point spread function (PSF).
        MatrixReal itsMatrix;
      };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
