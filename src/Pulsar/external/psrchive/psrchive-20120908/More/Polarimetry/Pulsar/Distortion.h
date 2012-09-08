//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Distortion_h
#define __Pulsar_Distortion_h

#include "Reference.h"

namespace Pulsar {

  class Archive;

  //! Analysis of the impact of instrumental boost on phase shift estimate
  class Distortion {

    Reference::To<Archive> standard;

  public:

    //! Set the standard (template) used to estimate phase shift
    void set_standard (Archive*);

    //! Set the calibrator (reference) used to estimate distortion
    void set_calibrator (Archive*);

  };
}

#endif
