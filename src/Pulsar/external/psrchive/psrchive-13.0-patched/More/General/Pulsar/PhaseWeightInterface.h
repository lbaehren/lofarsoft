//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeightInterface.h,v $
   $Revision: 1.1 $
   $Date: 2008/11/12 07:45:26 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseWeightTI_h
#define __Pulsar_PhaseWeightTI_h

#include "Pulsar/PhaseWeight.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Archive statistics text interface
  class PhaseWeight::Interface : public TextInterface::To<PhaseWeight>
  {
  public:

    //! Default constructor
    Interface ( PhaseWeight* = 0 );

  };

}


#endif
