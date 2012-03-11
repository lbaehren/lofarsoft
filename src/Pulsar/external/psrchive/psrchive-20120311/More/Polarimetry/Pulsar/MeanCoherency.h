//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanCoherency.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_MeanCoherency_H
#define __Calibration_MeanCoherency_H

#include "MEAL/Mean.h"
#include "MEAL/Complex2.h"
#include "Stokes.h"

namespace Calibration {

  //! A weighted mean of coherency matrix estimates
  class MeanCoherency : public MEAL::Mean<MEAL::Complex2> {

  public:

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current value of the mean
    void update (MEAL::Complex2* model) const;

    //! Add the Stokes parameter Estimate to the running mean
    void integrate (const Stokes< Estimate<double> >& stokes);

    //! Add the mean Stokes parameters
    Stokes< Estimate<double> > get_mean () const;

  protected:

    //! Running mean of Stokes parameters
    Stokes< MeanEstimate<double> > mean;

  };

}

#endif
