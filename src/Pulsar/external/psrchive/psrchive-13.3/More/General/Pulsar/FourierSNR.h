//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/FourierSNR.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_FourierSNR_h
#define __Pulsar_FourierSNR_h

#include "ReferenceAble.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio in the Fourier domain
  class FourierSNR : public Reference::Able {

  public:

    //! Default constructor
    FourierSNR ();

    //! Set the fractional number of high frequencies used to calculate noise
    void set_baseline_extent (float width);

    //! Return the signal to noise ratio
    float get_snr (const Profile* profile);

  protected:

    //! The fractional number of high frequencies used to calculate noise
    float baseline_extent;

  };

}

#endif
