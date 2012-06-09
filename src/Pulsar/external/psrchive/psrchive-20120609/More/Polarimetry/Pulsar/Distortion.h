//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_Distortion_h
#define __Pulsar_Distortion_h

namespace Pulsar {

  class Archive;

  //! Analysis of the impact of instrumental boost on phase shift estimate
  class Distortion {

  public:

    void set_calibrator (Archive* archive);

  };
}

#endif
