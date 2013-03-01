//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/InstallInterpreter.h,v $
   $Revision: 1.1 $
   $Date: 2007/06/12 05:53:18 $
   $Author: straten $ */

#ifndef __Pulsar_InstallInterpreter_h
#define __Pulsar_InstallInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class InstallInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    InstallInterpreter ();

    //! Destructor
    ~InstallInterpreter ();

    //! Install new pulsar parameters
    std::string parameters (const std::string& args);

    //! Install new pulse phase predictor
    std::string predictor (const std::string& args);

    //! Install new receiver parameters
    std::string receiver (const std::string& args);

    //! No empty arguments
    std::string empty ();

  };

}

#endif
