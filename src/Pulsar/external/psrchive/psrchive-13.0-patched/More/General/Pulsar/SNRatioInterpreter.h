//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/SNRatioInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/02 05:19:21 $
   $Author: straten $ */

#ifndef __Pulsar_SNRatioInterpreter_h
#define __Pulsar_SNRatioInterpreter_h

#include "CommandParser.h"
#include "Functor.h"

namespace Pulsar {

  class StandardSNR;
  class Profile;

  class SNRatioInterpreter : public CommandParser {

  public:

    //! Default constructor
    SNRatioInterpreter ();

    //! Install the adaptive S/N algorithm
    std::string adaptive (const std::string& args);

    //! Install the fourier S/N algorithm
    std::string fourier (const std::string& args);

    //! Install the square wave S/N algorithm
    std::string square (const std::string& args);

    //! Install the standard profile S/N algorithm
    std::string standard (const std::string& args);

    //! Install the default S/N algorithm
    std::string phase (const std::string& args);

    //! No empty arguments
    std::string empty ();

  protected:

    Reference::To<StandardSNR> standard_snratio;

    Functor< float (const Profile*) > adaptive_functor;
    Functor< float (const Profile*) > fourier_functor;
    Functor< float (const Profile*) > square_functor;
    Functor< float (const Profile*) > standard_functor;
    Functor< float (const Profile*) > phase_functor;

  };

}

#endif
