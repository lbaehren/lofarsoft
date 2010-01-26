//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SNRWeight.h,v $
   $Revision: 1.3 $
   $Date: 2008/08/05 13:30:53 $
   $Author: straten $ */

#ifndef _Pulsar_SNRWeight_H
#define _Pulsar_SNRWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Profile according to its S/N squared
  class SNRWeight : public IntegrationWeight {

  public:

    //! Default constructor
    SNRWeight ();

    //! Set weight to zero if S/N falls below threshold
    void set_threshold (float f) { threshold = f; }
    float get_threshold () const { return threshold; }

    //! Get the weight of the specified channel
    double get_weight (const Integration* integration, unsigned ichan);

  protected:

    float threshold;

  };
  
}

#endif

