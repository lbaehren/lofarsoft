//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/resources/Pulsar/GeneratorInterpreter.h,v $
   $Revision: 1.2 $
   $Date: 2009/11/29 12:13:50 $
   $Author: straten $ */

#ifndef __Pulsar_GeneratorInterpreter_h
#define __Pulsar_GeneratorInterpreter_h

#include "Pulsar/Generator.h"
#include "CommandParser.h"

namespace Pulsar
{

  //! Interprets configuration strings into the default generator
  class Generator::Interpreter : public CommandParser
  {

  public:

    Interpreter ();

    //! Install the interative Gaussian baseline algorithm
    std::string polyco (const std::string& args);

    //! Install the default baseline algorithm (minimum window)
    std::string tempo2 (const std::string& args);

    //! Return the configuration string of the current baseline policy
    std::string empty ();

    static Configuration::Parameter<Generator*>& get_option ();

  protected:

    std::string current;

  };

}

#endif
