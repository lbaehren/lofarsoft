//#  QualityMetrics.cc: quality metrics for the PSF estimation.
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
//#  $Id: QualityMetrics.cc,v 1.4 2006/05/19 16:03:24 loose Exp $

#include <Sinfoni/PSF/QualityMetrics.h>
#include <Sinfoni/PSF/InputData.h>
#include <Sinfoni/PSF/PointSpreadFunction.h>
#include <Sinfoni/PSF/VLTPupil.h>
#include <Sinfoni/PSF/ApertureToFriedRatio.h>
#include <Sinfoni/PSF/ModalCoordinates.h>
#include <Sinfoni/PSF/PropagatedMeasurementError.h>
#include <Sinfoni/PSF/RemainingError.h>
#include <Sinfoni/PSF/ModalCoordResidualPhase.h>
#include <Sinfoni/PSF/PSFConfig.h>
#include <Sinfoni/Assert.h>
#include <Sinfoni/Exceptions.h>
#include <Sinfoni/FFT.h>
#include <cerrno>

using namespace blitz;
using namespace std;

namespace ASTRON
{
  namespace SINFONI
  {
    namespace PSF
    {

      QualityMetrics::QualityMetrics() :
        itsNrOfSamples(nrOfSamples()),
        itsReconstructedStrehlRatio(reconstructedStrehlRatio()),
        itsEstimatedFriedParameter(estimatedFriedParameter()),
        itsEstimatedUncorrectedPhase(estimatedUncorrectedPhase()),
        itsEstimatedWfsMeasurementError(estimatedWfsMeasurementError()),
        itsEstimatedRemainingError(estimatedRemainingError()),
        itsEstimatedResidualPhase(estimatedResidualPhase()),
        itsRtcEstimatedResidualPhase(rtcEstimatedResidualPhase())
      {
      }
      
      Integer QualityMetrics::nrOfSamples()
      {
        return theInputData().nrOfSamples();
      }


      Real QualityMetrics::reconstructedStrehlRatio()
      {
        //# Create reference copies of the VLT pupil and the reconstructed
        //# PSF for easier access.
        MatrixInteger pupil(theVLTPupil().mask());
        MatrixReal psf(thePointSpreadFunction());

        //# Make sure that the matrix containing the VLT pupil is not larger
        //# than the matrix containing the reconstructed normalized PSF.
        //# This should never happen normally.
        Assert(pupil.rows() <= psf.rows() &&
               pupil.cols() <= psf.cols());

        //# Create a matrix \a optimalPsf for storing the diffraction
        //# limited PSF, having the same size as the reconstructed
        //# normalized PSF.
        MatrixReal optimalPsf(psf.shape());

        //# Place the VLT pupil in the center of this matrix.
        Integer lo((psf.rows()-pupil.rows())/2);
        Integer hi(lo+pupil.rows()-1);
        optimalPsf(Range(lo,hi), Range(lo,hi)) = cast<Real>(pupil);

        //# Calculate the spectral power of the diffraction limited PSF.
        //# Create a temporary complex matrix, because, currently, the FFT
        //# routines are only implemented for complex arrays and matrices.
        MatrixComplex tmp(cast<Complex>(optimalPsf));
        optimalPsf = sqr(abs(fftshift(fft2(tmp))));

        //# Calculate the Strehl ratio for the reconstructed PSF and return
        //# it.
        return (max(psf) / sum(psf)) * (sum(optimalPsf) / max(optimalPsf));
      }


      Real QualityMetrics::estimatedFriedParameter()
      {
        //# The estimated Fried parameter.
        Real r0(theVLTPupil().diameter() / theApertureToFriedRatio());

        //# Calculate r0 at 500 nm wavelength.
        r0 *= pow(Real(500e-9)/theVLTWaveLength, Real(6.0/5.0));
        if (errno) THROW (MathException, strerror(errno));

        //# Return the Fried parameter at 500 nm.
        return r0;
      }


      Real QualityMetrics::estimatedUncorrectedPhase()
      {
        //# The variance of all mirror modes for the estimated Fried
        //# parameter.
        VectorReal 
          var_a(ModalCoordinates::variance(theApertureToFriedRatio()));
          
        //# Sum only over the mirror modes used for estimation.
        Real var(sum(var_a(Range(1, theNrOfUsedMirrorModes-1))));

        //# The estimated Fried parameter.
        Real r0(theVLTPupil().diameter() / theApertureToFriedRatio());

        //# Calculate total variance at 500 nm wavelength.
        var *= pow(r0/estimatedFriedParameter(), Real(5.0/3.0));
        if (errno) THROW (MathException, strerror(errno));

        //# Return the estimated uncorrected phase at 500 nm wavelength.
        return var;
      }


      Real QualityMetrics::estimatedWfsMeasurementError()
      {
        //# The variance of the propagated WFS measurement error for all
        //# mirror modes.
        VectorReal var_n(diag(thePropagatedMeasurementError().covariance()));

        //# Sum only over the mirror modes used for estimation.
        Real var(sum(var_n(Range(1, theNrOfUsedMirrorModes-1))));

        //# Return the estimated WFS measurement error.
        return var;
      }


      Real QualityMetrics::estimatedRemainingError()
      {
        //# The variance of the remaining (or aliasing) error for all mirror
        //# modes.
        VectorReal var_r(diag(theRemainingError().
                              covariance(theApertureToFriedRatio())));

        //# Sum only over the mirror modes used for estimation.
        Real var(sum(var_r(Range(1, theNrOfUsedMirrorModes-1))));

        //# The estimated Fried parameter.
        Real r0(theVLTPupil().diameter() / theApertureToFriedRatio());
          
        //# Calculate total variance at 500 nm wavelength.
        var *= pow(r0/estimatedFriedParameter(), Real(5.0/3.0));
        if (errno) THROW (MathException, strerror(errno));

        //# Return the estimated remaining (or aliasing) error at 500 nm
        //# wavelength.
        return var;
      }


      Real QualityMetrics::estimatedResidualPhase()
      {
        //# The variance of the modal coordinates of the residual phase.
        VectorReal var_c(diag(theModalCoordResidualPhase().covariance()));

        //# Sum only over the mirror modes used for estimation.
        Real var(sum(var_c(Range(1, theNrOfUsedMirrorModes-1))));

        //# Return the estimated residual phase.
        return var;
      }


      Real QualityMetrics::rtcEstimatedResidualPhase()
      {
        //# The variance of the real-time computer (RTC) estimate of the modal
        //# coordinates of the residual phase.
        VectorReal 
          var_ce(diag(theModalCoordResidualPhase().covarianceEstimate()));

        //# Sum only over the mirror modes used for estimation.
        Real var(sum(var_ce(Range(1, theNrOfUsedMirrorModes-1))));

        //# Return the RTC estimated residual phase.
        return var;
      }


      const QualityMetrics& theQualityMetrics()
      {
        static QualityMetrics qm;
        return qm;
      }


      ostream& operator<<(ostream& os, const QualityMetrics& qm)
      {
        os << endl
           << "Number of samples used to reconstruct the PSF: " << setw(9)
           << qm.itsNrOfSamples                                 << endl
           << "Reconstructed Strehl ratio:                    " << setw(9)
           << qm.itsReconstructedStrehlRatio                    << endl
           << "Estimated r0 (m) at 500 nm:                    " << setw(9)
           << qm.itsEstimatedFriedParameter        << " m"      << endl
           << "Estimated uncorrected phase at 500 nm:         " << setw(9)
           << qm.itsEstimatedUncorrectedPhase      << " rad^2"  << endl
           << "Estimated WFS measurement error:               " << setw(9)
           << qm.itsEstimatedWfsMeasurementError   << " rad^2"  << endl
           << "Estimated aliasing error at 500 nm:            " << setw(9)
           << qm.itsEstimatedRemainingError        << " rad^2"  << endl
           << "Estimated residual phase:                      " << setw(9)
           << qm.itsEstimatedResidualPhase         << " rad^2"  << endl
           << "RTC estimated residual phase:                  " << setw(9)
           << qm.itsRtcEstimatedResidualPhase      << " rad^2"  << endl
           << endl;
        return os;
      }

    } // namespace PSF
    
  } // namespace SINFONI
  
} // namespace ASTRON
