//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/FluxCentroid.h,v $
   $Revision: 1.2 $
   $Date: 2009/10/02 03:40:45 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCentroid_h
#define __Pulsar_FluxCentroid_h

#include "Pulsar/ProfileShiftEstimator.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  //! Estimates phase shift using the flux "centre of mass"
  class FluxCentroid : public ProfileShiftEstimator
  {

  public:

    //! Default constructor
    FluxCentroid ();

    //! Return the shift estimate
    Estimate<double> get_shift () const;

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface () { return 0; }

    //! Return a copy constructed instance of self
    FluxCentroid* clone () const { return new FluxCentroid(*this); }

  protected:

    //! Duty cycle of pulse, over which flux will be integrated
    float duty_cycle;
  };

}


#endif // !defined __Pulsar_FluxCentroid_h
