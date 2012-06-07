//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SmoothMedian.h,v $
   $Revision: 1.5 $
   $Date: 2007/10/12 02:45:16 $
   $Author: straten $ */

#ifndef __Pulsar_SmoothMedian_h
#define __Pulsar_SmoothMedian_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a Profile using the median over a boxcar
  /*! 
    This transformation smooths the profile by setting each amplitude
    equal to the median calculated over the region centred at that point
    and with width specified as bins or turns.
  */
  class SmoothMedian : public Smooth {

  public:

    void transform (Profile*);

  };

}

#endif
