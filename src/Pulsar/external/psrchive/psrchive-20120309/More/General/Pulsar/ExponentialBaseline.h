//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ExponentialBaseline.h,v $
   $Revision: 1.7 $
   $Date: 2009/06/24 05:02:23 $
   $Author: straten $ */

#ifndef __Pulsar_ExponentialBaseline_h
#define __Pulsar_ExponentialBaseline_h

#include "Pulsar/IterativeBaseline.h"

namespace Pulsar {

  //! Finds a baseline that contains gaussian white noise
  class ExponentialBaseline : public IterativeBaseline {

  public:

    //! Default constructor
    ExponentialBaseline ();

    //! Set the threshold below which samples are included in the baseline
    void set_threshold (float sigma);
    float get_threshold () const { return threshold; }

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    ExponentialBaseline* clone () const;

  protected:

    //! Adjustment when computing variance of samples below threshold
    float moment_correction;

    void get_bounds (PhaseWeight* weight, float& lower, float& upper);

  };

}

#endif
