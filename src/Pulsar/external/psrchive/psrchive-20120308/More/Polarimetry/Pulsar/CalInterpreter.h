//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/CalInterpreter.h,v $
   $Revision: 1.5 $
   $Date: 2009/06/25 05:28:25 $
   $Author: straten $ */

#ifndef __Pulsar_CalInterpreter_h
#define __Pulsar_CalInterpreter_h

#include "Pulsar/InterpreterExtension.h"
#include "Pulsar/Calibrator.h"
#include "MEAL/Complex2.h"

namespace Pulsar {

  class Database;
  class PolnCalibrator;
  
  class CalInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    CalInterpreter ();

    //! Destructor
    ~CalInterpreter ();

    // calibrate the current top of the stack using the current state
    std::string cal (const std::string& args);

    //! cal command interface
    std::string type (const std::string& args);

    //! load the specifed file (database or calibrator)
    std::string load (const std::string& arg);

    //! perform the frontend (basis and projection) correction
    std::string frontend (const std::string& arg);

  protected:

    //! The type of calibrator to be used
    Reference::To<const Calibrator::Type> caltype;

    //! Calibrator database
    Reference::To<Database> database;

    //! Calibrator to be applied
    Reference::To<PolnCalibrator> calibrator;

    //! The transformation to be applied
    Reference::To<MEAL::Complex2> transformation;

  };

}

#endif
