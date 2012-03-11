//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanFeed.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_MeanFeed_H
#define __Calibration_MeanFeed_H

#include "MEAL/Mean.h"
#include "MEAL/Complex2.h"

namespace Calibration {

  class Feed;

  //! A weighted mean of Feed parameter estimates
  class MeanFeed : public MEAL::Mean<MEAL::Complex2> {

  public:

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current mean
    void update (MEAL::Complex2* model) const;

    //! Return the difference between the mean and the model
    double chisq (const MEAL::Complex2* model) const;

    //! Add the Model parameters to the running mean
    void integrate (const Feed* model);

    //! Update the Model parameters with the current mean
    void update (Feed* model) const;

    //! Return the difference between the mean and the Feed
    double chisq (const Feed* model) const;

  protected:

    //! Best estimate of receptor orientations
    MeanRadian<double> mean_orientation[2];

    //! Best estimate of receptor ellipticities
    MeanRadian<double> mean_ellipticity[2];

  };

}

#endif
