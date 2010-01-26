//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PatchTime.h,v $
   $Revision: 1.1 $
   $Date: 2009/11/19 11:17:22 $
   $Author: straten $ */

#ifndef _Pulsar_PatchTime_H
#define _Pulsar_PatchTime_H

#include "Pulsar/Contemporaneity.h"

namespace Pulsar {
  
  class Archive;

  //! Fills missing sub-integrations with empties
  class PatchTime : public Algorithm 
  {
    
  public:
    
    //! Default constructor
    PatchTime ();

    //! Add to A whatever is missing with respect to B, and vice versa
    void operate (Archive* A, Archive* B);

    //! Set the policy used to judge contemporaneity
    void set_contemporaneity_policy (Contemporaneity*);

    //! Set the minimum contemporaneity required for each sub-integration
    void set_minimum_contemporaneity (double min);

  protected:

    Reference::To<Contemporaneity> contemporaneity_policy;

    double minimum_contemporaneity;
  };
  
}

#endif
