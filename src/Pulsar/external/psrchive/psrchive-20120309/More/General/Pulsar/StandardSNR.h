//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/StandardSNR.h,v $
   $Revision: 1.7 $
   $Date: 2011/01/11 01:16:54 $
   $Author: straten $ */

#ifndef __Pulsar_StandardSNR_h
#define __Pulsar_StandardSNR_h

#include "Pulsar/ProfileShiftFit.h"

namespace Pulsar {

  class Profile;

  //! Calculates the signal-to-noise ratio by fitting against a standard
  class StandardSNR : public Reference::Able {

  public:

    //! Set the standard against which the S/N will be calculated
    void set_standard (const Profile* profile);

    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);

  protected:

    //! Used to perform the fit and compute the S/N
    ProfileShiftFit fit;
  };

}

#endif
