//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/FITSAlias.h,v $
   $Revision: 1.1 $
   $Date: 2006/11/29 06:07:09 $
   $Author: straten $ */

#ifndef __FITSAlias_h
#define __FITSAlias_h

#include "Alias.h"

namespace Pulsar {

  //! PSRFITS parameter names for Archive TextInterface attribute names
  class FITSAlias : public Alias
  {
  public:

    //! Constructor
    FITSAlias ();

  };

}

#endif

