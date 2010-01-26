//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SmoothSinc.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/12 02:45:44 $
   $Author: straten $ */

#ifndef __Pulsar_SmoothSinc_h
#define __Pulsar_SmoothSinc_h

#include "Pulsar/Smooth.h"

namespace Pulsar {

  //! Smooths a Profile by convolution with a sinc function
  /*! 
    This method smooths the profile by low-pass filtering
    
  */
  class SmoothSinc : public Smooth {

  public:

    void transform (Profile*);

  };

}

#endif
