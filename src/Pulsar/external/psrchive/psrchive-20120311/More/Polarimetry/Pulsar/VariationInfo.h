//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/VariationInfo.h,v $
   $Revision: 1.3 $
   $Date: 2008/04/07 00:38:18 $
   $Author: straten $ */

#ifndef __Pulsar_VariationInfo_H
#define __Pulsar_VariationInfo_H

#include "Pulsar/SystemCalibrator.h"
#include "MEAL/Scalar.h"

namespace Pulsar {

  //! Communicates time variation parameters to plotting routines
  class VariationInfo : public Calibrator::Info {

  public:

    //! Things that vary in a SystemCalibrator
    enum Which {
      //! Absolute gain
      Gain,
      //! Differential gain
      Boost,
      //! Differential phase
      Rotation
    };

    //! Construct with which function to plot
    VariationInfo (const SystemCalibrator* calibrator, Which which);
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
    //! Return the Scalar variation function for the specified channel
    const MEAL::Scalar* get_Scalar (unsigned ichan) const;

  protected:

    //! The SystemCalibrator containing the step functions
    Reference::To<const SystemCalibrator> calibrator;

    //! Which function to plot
    Which which;

  };

}

#endif
