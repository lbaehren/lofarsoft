//#  QualityMetrics.h: quality metrics for the PSF estimation.
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
//#  $Id: QualityMetrics.h,v 1.3 2006/05/19 16:03:24 loose Exp $

#ifndef SINFONI_PSF_QUALITYMETRICS_H
#define SINFONI_PSF_QUALITYMETRICS_H

#include <Sinfoni/Config.h>

//# Includes
#include <Sinfoni/Types.h>
#include <iostream>

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {
      // This class combines the most important quality metrics for the PSF
      // estimation.
      class QualityMetrics
      {
      private:
        // The friend function is used to return an instance of the
        // QualityMetrics class.
        friend const QualityMetrics& theQualityMetrics();

        // Pretty-print the quality metrics onto the given output stream.
        friend std::ostream& operator<<(std::ostream& os, 
                                        const QualityMetrics& qm);

        //@{
        // Disallow construction, copying and assignment. Construction is done
        // by the friend function.
        QualityMetrics();
        QualityMetrics(const QualityMetrics&);
        QualityMetrics& operator=(const QualityMetrics&);
        //@}
 
        // Number of samples used to reconstruct the PSF
        Integer nrOfSamples();

        // Return the reconstructed Strehl ratio, which is defined as the
        // ratio of the reconstructed PSF and the diffraction limited PSF.
        Real reconstructedStrehlRatio();

        // Return the estimated Fried parameter \f$r_0\f$ at 500 nm
        // wavelength.
        Real estimatedFriedParameter();

        // Return the estimated uncorrected phase \f$\sum\sigma^2_{a_i}\f$ in
        // rad<sup>2</sup> at 500 nm wavelength.
        Real estimatedUncorrectedPhase();

        // Return the estimated WFS measurement error \f$\sum\sigma^2_{n_i}\f$
        // in rad<sup>2</sup>.
        Real estimatedWfsMeasurementError();

        // Return the estimated remaining (or aliasing) error
        // \f$\sum\sigma^2_{r_i}\f$ in rad<sup>2</sup> at 500 nm wavelength.
        Real estimatedRemainingError();

        // Return the estimated residual phase \f$\sum\sigma^2_{\epsilon_i}\f$
        // in rad<sup>2</sup>.
        Real estimatedResidualPhase();

        // Return the RTC estimated residual phase
        // \f$\sum\sigma^2_{{\hat{\epsilon}}_i}\f$ in rad<sup>2</sup>.
        Real rtcEstimatedResidualPhase();

        // Number of samples used to reconstruct the PSF.
        Integer itsNrOfSamples;

        // The reconstructed Strehl ratio.
        Real itsReconstructedStrehlRatio;

        // The estimated Fried parameter.
        Real itsEstimatedFriedParameter;

        // The estimated uncorrected phase.
        Real itsEstimatedUncorrectedPhase;

        // The estimated WFS measurement error.
        Real itsEstimatedWfsMeasurementError;

        // The estimated remaining (or aliasing) error.
        Real itsEstimatedRemainingError;

        // The estimated residual phase.
        Real itsEstimatedResidualPhase;

        // The RTC estimated residual phase.
        Real itsRtcEstimatedResidualPhase;

      };
      
    } // namespace PSF

  } // namespace SINFONI

} // namespace ASTRON

#endif
