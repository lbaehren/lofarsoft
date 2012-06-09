//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/OffPulseCalibrator.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Pulsar_OffPulseCalibrator_H
#define __Pulsar_OffPulseCalibrator_H

#include "Pulsar/SingleAxisCalibrator.h"

namespace Pulsar {

  //! Uses the off-pulse noise statistics to scale the polarization
  /*! This calibrator should probably never be used.  It was developed to
    deal with Wideband Correlator data observed during a period of heavy
    testing and development (P236).  It was observed that the system does
    not remain stable between calibrator and pulsar observations, nor from
    sub-integration to sub-integration.  Therefore, this calibrator attempts
    to correct the gains using the statistics of the off-pulse baseline.
    The error in the assumption that this baseline is unpolarized is the
    least of our concerns at this point.
  */
  class OffPulseCalibrator : public SingleAxisCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    OffPulseCalibrator (const Archive* archive);

    //! Destructor
    ~OffPulseCalibrator ();

    //! Calibrate the polarization of the given archive
    void calibrate (Archive* archive);

  protected:

    //! Disable the differential gain correction
    void extra (unsigned ichan,
		const std::vector< Estimate<double> >& source,
		const std::vector< Estimate<double> >& sky);

  };

}


#endif
