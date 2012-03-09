/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileInterpreter.h"
#include "Pulsar/ZapInterpreter.h"
#include "Pulsar/CalInterpreter.h"
#include "Pulsar/InstallInterpreter.h"

Pulsar::Interpreter* standard_shell ()
{
  Reference::To<Pulsar::Interpreter> interpreter = new Pulsar::Interpreter;

  interpreter->import( new Pulsar::ProfileInterpreter,
		       "profile", "profile transformations" );

  interpreter->import( new Pulsar::ZapInterpreter,
		       "zap", "zap data using the specified method" );

  interpreter->import( new Pulsar::CalInterpreter,
		       "cal", "polarimetric calibration" );

  interpreter->import( new Pulsar::InstallInterpreter,
		       "install", "install auxilliary data" );

  return interpreter.release();
}

