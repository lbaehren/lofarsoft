//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileWeightStatic.h,v $
   $Revision: 1.1 $
   $Date: 2011/03/04 06:01:57 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileWeightStatic_h
#define __Pulsar_ProfileWeightStatic_h

#include "Pulsar/ProfileWeightFunction.h"

namespace Pulsar
{
  //! Set the Profile weights to a user-specified value
  class ProfileWeightStatic : public ProfileWeightFunction
  {

  public:

    //! Return a text interface that can be used to configure this instance
    TextInterface::Parser* get_interface ();

    //! Interface to the algorithm
    class Interface;

    //! Return a copy constructed instance of self
    ProfileWeightStatic* clone () const;

    //! Set the profile phase bins
    void set_bins (const std::string&);
    std::string get_bins () const;

    //! Set the profile phase ranges in turns
    void set_turns (const std::string&);
    std::string get_turns () const;

  protected:

    //! Calculate the PhaseWeight
    void calculate (PhaseWeight* weight);

    std::string bins;
    std::string turns;

  };

}

#endif
