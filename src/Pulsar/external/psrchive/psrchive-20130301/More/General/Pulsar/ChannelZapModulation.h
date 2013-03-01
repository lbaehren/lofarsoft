//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ChannelZapModulation.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef _Pulsar_ChannelZapModulation_H
#define _Pulsar_ChannelZapModulation_H

#include "Pulsar/ChannelWeight.h"

namespace Pulsar {
  
  //! Weights each channel using normalized r.m.s. total intensity
  class ChannelZapModulation : public ChannelWeight {
    
  public:
    
    //! Default constructor
    ChannelZapModulation ();

    //! Set integration weights
    void weight (Integration* integration);

  protected:

    float cutoff_threshold;

  };
  
}

#endif
