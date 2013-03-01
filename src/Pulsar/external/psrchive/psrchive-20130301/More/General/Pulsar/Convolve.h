//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/More/General/Pulsar/Convolve.h

#ifndef __Pulsar_Convolve_h
#define __Pulsar_Convolve_h

#include "Pulsar/Combination.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Computes the difference between two profiles
  class Convolve : public Combination<Profile>
  {

  public:

    void transform (Profile*);

  }; 

}

#endif
