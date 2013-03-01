//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/WeightInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2008/08/05 13:06:44 $
   $Author: straten $ */

#ifndef __Pulsar_WeightInterpreter_h
#define __Pulsar_WeightInterpreter_h

#include "CommandParser.h"
#include "Functor.h"

namespace Pulsar {

  class Integration;

  //! Interprets configuration strings into a weighting policy

  class WeightInterpreter : public CommandParser {

  public:

    typedef Functor< void (Integration*) > Policy;

    //! Construct with reference to the policy to be modified
    WeightInterpreter (Policy&);

    //! Install the RadiometerWeight policy
    std::string radiometer (const std::string& args);

    //! Install the DurationWeight policy
    std::string time (const std::string& args);

    //! Install the SNRWeight policy
    std::string snr (const std::string& args);

    //! Disable weighting
    std::string none (const std::string& args);

    //! Return the configuration string of the current weighting policy
    std::string empty ();

  protected:

    //! The radiometer equation weighting policy
    Policy radiometer_functor;

    //! The duration weighting policy
    Policy time_functor;

    //! The snr weighting policy
    Policy snr_functor;

    //! The policy to be modified
    Policy& policy;

  };

}

#endif
