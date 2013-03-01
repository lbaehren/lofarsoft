//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/StandardSNRWeight.h,v $
   $Revision: 1.1 $
   $Date: 2011/01/11 12:43:28 $
   $Author: straten $ */

#ifndef _Pulsar_StandardSNRWeight_H
#define _Pulsar_StandardSNRWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  class ProfileShiftFit;

  //! Sets the weight of each Profile according to its S/N squared
  class StandardSNRWeight : public IntegrationWeight {

  public:

    //! Construct from standard profile filename
    StandardSNRWeight (const std::string& filename);

    //! Set the weights of all Profiles in the Archive
    void operator () (Archive*);

    //! Get the weight of the specified channel
    double get_weight (const Integration* integration, unsigned ichan);

  protected:

    Reference::To<ProfileShiftFit> total;
    std::vector< Reference::To<ProfileShiftFit> > chan;

  private:
    bool use_chan;

  };
  
}

#endif

