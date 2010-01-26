//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineInterpreter.h,v $
   $Revision: 1.3 $
   $Date: 2008/08/05 13:30:48 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineInterpreter_h
#define __Pulsar_BaselineInterpreter_h

#include "CommandParser.h"
#include "Functor.h"

namespace Pulsar {

  class PhaseWeight;
  class Profile;

  //! Interprets configuration strings into a weighting policy

  class BaselineInterpreter : public CommandParser {

  public:

    typedef Functor< PhaseWeight* (const Profile*) > Policy;

    //! Construct with reference to the policy to be modified
    BaselineInterpreter (Policy&);

    //! Install the interative Gaussian baseline algorithm
    std::string normal (const std::string& args);

    //! Install the default baseline algorithm (minimum window)
    std::string minimum (const std::string& args);

    //! Return the configuration string of the current baseline policy
    std::string empty ();

  protected:

    //! The normal baseline policy (GaussianBaseline)
    Policy normal_functor;

    //! The minimum mean baseline policy (BaselineWindow)
    Policy minimum_functor;

    //! The policy to be modified
    Policy& policy;

  };

}

#endif
