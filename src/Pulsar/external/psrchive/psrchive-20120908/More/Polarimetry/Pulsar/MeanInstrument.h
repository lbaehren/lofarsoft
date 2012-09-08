//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanInstrument.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_MeanInstrument_H
#define __Calibration_MeanInstrument_H

#include "Pulsar/MeanSingleAxis.h"
#include "Pulsar/MeanFeed.h"

namespace Calibration {

  class MeanSingleAxis;
  class MeanFeed;

  //! A weighted mean of Instrument parameter estimates
  class MeanInstrument : public MEAL::Mean<MEAL::Complex2> {

  public:

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current value of the mean
    void update (MEAL::Complex2* model) const;

    //! Return a measure of the difference between the mean and model
    double chisq (const MEAL::Complex2* model) const;

  protected:

    //! The mean values of the SingleAxis model parameters
    MeanSingleAxis single_axis;

    //! The mean values of the Feed model parameters
    MeanFeed feed;

  };

}

#endif
