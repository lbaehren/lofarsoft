//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* More/Timing/Pulsar/ProfileShiftEstimator.h */

#ifndef __Pulsar_PolnProfileShiftEstimator_h
#define __Pulsar_PolnProfileShiftEstimator_h

#include "Pulsar/ShiftEstimator.h"
#include "Pulsar/PolnProfile.h"

namespace Pulsar {

  //! Algorithms that estimate phase shift from a single polarization profile
  class PolnProfileShiftEstimator : public ShiftEstimator
  {

  public:

    //! Set the PolnProfile from which the shift will be estimated
    void set_observation (const PolnProfile* p) { observation = p; }

    //! Get the PolnProfile from which the shift will be estimated
    const PolnProfile* get_observation () const { return observation; }

  protected:

    Reference::To<const PolnProfile> observation;
  };

}


#endif // !defined __Pulsar_PolnProfileShiftEstimator_h
