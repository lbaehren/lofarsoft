//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeanJones.h,v $
   $Revision: 1.1 $
   $Date: 2010/01/20 05:11:44 $
   $Author: straten $ */

#ifndef __Calibration_MeanJones_H
#define __Calibration_MeanJones_H

#include "MEAL/JonesCloude.h"

namespace Calibration {

  //! Computes a weighted mean Jones matrix using Cloude target decomposition
  class MeanJones : public Reference::Able
  {

  public:

    static bool verbose;

    //! Add the Model parameters to the running mean
    void integrate (const MEAL::Complex2* model);

    //! Update the Model parameters with the current value of the mean
    Jones<double> get_mean ();

  protected:

    Matrix<4,4, std::complex< MeanEstimate<double> > > target_coherency;
    MEAL::JonesCloude convert;

  };

}

#endif
