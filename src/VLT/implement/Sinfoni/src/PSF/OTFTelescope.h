//#  OTFTelescope.h: OTF of the telescope.
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
//#  $Id: OTFTelescope.h,v 1.3 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_OTFTELESCOPE_H
#define SINFONI_PSF_OTFTELESCOPE_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/PSF/VLTPupil.h>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This class represents the optical transfer function of the telescope
      // \f$\mathbf{B}_{tel}\f$, in the absence of turbulence.
      // \f[
      //   \mathbf{B}_{tel}(\boldsymbol{\rho}/\lambda) = 
      //     \frac{1}{S} \int\!\int_{\EuScript{P}} 
      //       P(\mathbf{r}) P(\mathbf{r} + \boldsymbol{\rho}) d\mathbf{r}
      // \f]
      // Note that, by definition, \f$\mathbf{B}_{tel}\f$ is equal to the
      // autocorrelation of the pupil of the telescope.
      class OTFTelescope
      {
      private:
        // Calculate the autocorrelation of the pupil. The autocorrelation is
        // calculated on a grid twice the size of that of the pupil in order
        // to avoid wrap-around effects. The result is stored in \c itsMatrix.
        // For speed efficiency, the autocorrelation is calculated using the
        // Wiener-Khinchin theorem i.e. through the Fourier domain.
        void autocorr();

        // The friend function is used to return the OTF of the telescope.
        friend const MatrixReal& theOTFTelescope();

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        OTFTelescope();
        OTFTelescope(const OTFTelescope&);
        OTFTelescope& operator=(const OTFTelescope&);
        //@}

        // Matrix containing the optical transfer function of the telescope.
        MatrixReal itsMatrix;
      };
      
      // Return the optical transfer function of the telescope.
      const MatrixReal& theOTFTelescope();
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
