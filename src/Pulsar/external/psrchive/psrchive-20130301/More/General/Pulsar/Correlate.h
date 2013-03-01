//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/psrchive/More/General/Pulsar/Correlate.h

#ifndef __Pulsar_Correlate_h
#define __Pulsar_Correlate_h

#include "Pulsar/Combination.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Computes the difference between two profiles
  class Correlate : public Combination<Profile>
  {

  public:

    //! By default, call Profile::correlate_normalized
    Correlate () { normalize = true; }

    void transform (Profile*);

  protected:

    bool normalize;
  }; 

}

#endif
