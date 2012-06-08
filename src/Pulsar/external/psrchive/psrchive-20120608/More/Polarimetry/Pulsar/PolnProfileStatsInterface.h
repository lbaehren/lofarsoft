//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PolnProfileStatsInterface.h,v $
   $Revision: 1.1 $
   $Date: 2008/11/12 07:45:41 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileStatsTI_h
#define __Pulsar_PolnProfileStatsTI_h

#include "Pulsar/PolnProfileStats.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Archive statistics text interface
  class PolnProfileStats::Interface 
    : public TextInterface::To<PolnProfileStats>
  {
  public:

    //! Default constructor
    Interface ( PolnProfileStats* = 0 );

  };

}


#endif
