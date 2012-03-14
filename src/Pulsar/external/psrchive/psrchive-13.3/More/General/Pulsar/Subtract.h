//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Subtract.h,v $
   $Revision: 1.2 $
   $Date: 2008/04/07 00:38:18 $
   $Author: straten $ */

#ifndef __Pulsar_Subtract_h
#define __Pulsar_Subtract_h

#include "Pulsar/Combination.h"
#include "Pulsar/Profile.h"

namespace Pulsar {

  //! Profile smoothing algorithms
  class Subtract : public Combination<Profile> {

  public:

    void transform (Profile*);

  }; 

}

#endif
