//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileStatsInterface.h,v $
   $Revision: 1.1 $
   $Date: 2008/11/12 07:45:41 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileStatsTI_h
#define __Pulsar_ProfileStatsTI_h

#include "Pulsar/ProfileStats.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Archive statistics text interface
  class ProfileStats::Interface : public TextInterface::To<ProfileStats>
  {
  public:

    //! Default constructor
    Interface ( ProfileStats* = 0 );

  };

}


#endif
