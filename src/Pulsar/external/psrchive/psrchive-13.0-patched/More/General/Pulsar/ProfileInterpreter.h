//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ProfileInterpreter.h,v $
   $Revision: 1.3 $
   $Date: 2007/12/24 20:02:08 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileInterpreter_h
#define __Pulsar_ProfileInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class Smooth;

  class ProfileInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    ProfileInterpreter ();

    //! Destructor
    ~ProfileInterpreter ();

    //! Remove the profile baseline
    std::string baseline (const std::string& args);

    //! Form the cummulative profile
    std::string cumulative (const std::string& args);

    //! Form the difference profile
    std::string difference (const std::string& args);

    //! Form the mean smoothed profile
    std::string mean (const std::string& args);

    //! Form the median smoothed profile
    std::string median (const std::string& args);

    //! Form the low-pass filtered profile
    std::string sinc (const std::string& args);

    //! Subtract profiles from the named archive
    std::string subtract (const std::string& args);

    //! No empty arguments
    std::string empty ();

  protected:

    std::string smooth (Smooth*, const std::string& args);

  };

}

#endif
