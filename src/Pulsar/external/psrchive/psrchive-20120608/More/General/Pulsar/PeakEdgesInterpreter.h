//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PeakEdgesInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/02 05:19:02 $
   $Author: straten $ */

#ifndef __Pulsar_PeakEdgesInterpreter_h
#define __Pulsar_PeakEdgesInterpreter_h

#include "CommandParser.h"
#include "Functor.h"

namespace Pulsar {

  class PhaseWeight;
  class Profile;

  class PeakEdgesInterpreter : public CommandParser {

  public:

    //! Default constructor
    PeakEdgesInterpreter ();

    //! Install the PeakCumulative find_peak_edges algorithm
    std::string cumulative (const std::string& args);

    //! Install the PeakConsecutive find_peak_edges algorithm
    std::string consecutive (const std::string& args);

    //! No empty arguments
    std::string empty ();

  protected:

    Functor< std::pair<int,int> (const Profile*) > cumulative_functor;
    Functor< std::pair<int,int> (const Profile*) > consecutive_functor;

  };

}

#endif
