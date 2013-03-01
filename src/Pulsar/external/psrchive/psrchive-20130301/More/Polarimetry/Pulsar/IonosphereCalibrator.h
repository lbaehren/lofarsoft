//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/IonosphereCalibrator.h,v $
   $Revision: 1.4 $
   $Date: 2009/03/01 18:04:42 $
   $Author: straten $ */

#ifndef __Pulsar_IonosphereCalibrator_H
#define __Pulsar_IonosphereCalibrator_H

#include "Pulsar/Calibrator.h"

namespace Pulsar {

  //! Corrects known instrumental effects
  class IonosphereCalibrator : public Calibrator {

  public:

    //! Default constructor
    IonosphereCalibrator ();

    //! Destructor
    ~IonosphereCalibrator ();

    // ///////////////////////////////////////////////////////////////////
    //
    // Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calibrate the Pulsar::Archive
    void calibrate (Archive* archive);

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const { return 0; }

    //! Returns a new CalibratorExtension
    CalibratorExtension* new_Extension () const { return 0; }
  };
}

#endif
