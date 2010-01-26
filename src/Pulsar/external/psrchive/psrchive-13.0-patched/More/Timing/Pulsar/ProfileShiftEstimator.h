//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Timing/Pulsar/ProfileShiftEstimator.h,v $
   $Revision: 1.1 $
   $Date: 2009/09/30 02:16:19 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileShiftEstimator_h
#define __Pulsar_ProfileShiftEstimator_h

#include "Pulsar/ShiftEstimator.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Algorithms that estimate the phase shift from a single pulse profile
  class ProfileShiftEstimator : public ShiftEstimator
  {

  public:

    //! Set the profile from which the shift will be estimated
    void set_observation (const Profile* p) { observation = p; }

    //! Get the profile from which the shift will be estimated
    const Profile* get_observation () const { return observation; }

  protected:

    Reference::To<const Profile> observation;
  };

}


#endif // !defined __Pulsar_ProfileShiftEstimator_h
