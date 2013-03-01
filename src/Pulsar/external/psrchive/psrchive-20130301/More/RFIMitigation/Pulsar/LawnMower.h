//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/RFIMitigation/Pulsar/LawnMower.h,v $
   $Revision: 1.5 $
   $Date: 2010/07/13 09:36:38 $
   $Author: straten $ */

#ifndef __Pulsar_LawnMower_h
#define __Pulsar_LawnMower_h

#include "Pulsar/Mower.h"

namespace Pulsar {

  class BaselineEstimator;
  class OnPulseThreshold;
  class PhaseWeight;

  //! Removes broad-band impulsive spikes from pulse profiles
  /*! This original version of the lawn mower algorithm makes use
   of various baseline estimators but is too sensitive to outliers.
   It is better to use the RobustMower class. */

  class LawnMower : public Mower
  {

  public:

    //! Default constructor
    LawnMower ();

    //! Destructor
    ~LawnMower ();

    void set_threshold (float sigma);

    //! Set the BaselineEstimator used to find the baseline
    void set_baseline_estimator (BaselineEstimator*);
    BaselineEstimator* get_baseline_estimator () const;

  protected:
    
    //! Masks all points above a threshold
    Reference::To<OnPulseThreshold> mower;

    //! Points to be included in baseline estimator used by mower
    Reference::To<PhaseWeight> include;

    //! Find the spikes in median smoothed difference and flag them in mask
    void compute (PhaseWeight* mask, const Profile* difference);

  };

}

#endif
