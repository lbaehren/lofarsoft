//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/HybridCalibrator.h,v $
   $Revision: 1.11 $
   $Date: 2009/03/01 18:04:42 $
   $Author: straten $ */

#ifndef __HybridCalibrator_H
#define __HybridCalibrator_H

#include "Pulsar/PolnCalibrator.h"

namespace Calibration {
  class SingleAxisSolver;
}

namespace Pulsar {

  class ReferenceCalibrator;
  class CalibratorStokes;

  //! Supplements a SystemCalibrator with a SingleAxisCalibrator
  class HybridCalibrator : public PolnCalibrator {
    
  public:

    //! Construct from another PolnCalibrator instance
    HybridCalibrator (PolnCalibrator* precalibrator = 0);

    //! Construct from a solution with both extensions
    HybridCalibrator (const Archive* data);

    //! Destructor
    ~HybridCalibrator ();

    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Set the Stokes parameters of the reference signal
    void set_reference_input (const CalibratorStokes* reference);

    //! Set the ReferenceCalibrator data from which to derive a SingleAxis
    void set_reference_observation (ReferenceCalibrator* observation);
    
    //! Set the PolnCalibrator to be supplemented
    void set_precalibrator (PolnCalibrator* calibrator);

  protected:

    //! Fill the transformation vector of the PolnCalibrator base class
    void calculate_transformation ();

    //! Return channel validity at the original resolution, if necessary
    bool get_valid (unsigned ichan) const;

    //! Return the number of channels in the PolnCalibrator
    unsigned get_maximum_nchan ();

    //! The Stokes parameters of the input reference signal
    Reference::To<const CalibratorStokes> reference_input;

    //! The Stokes parameters of the observed reference signal
    Reference::To<ReferenceCalibrator> reference_observation;

    //! The PolnCalibrator to be supplemented
    Reference::To<PolnCalibrator> precalibrator;

    //! The SingleAxis model solver
    Reference::To<Calibration::SingleAxisSolver> solver;

  };

}

#endif
