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
  /*
    By default, print/parse angles in degrees.

    It would be nice if this function didn't have to change a global
    variable, but at the moment I don't see any around this problem
    
    https://sourceforge.net/tracker/?func=detail&aid=3530973&group_id=159762&atid=813132

  */

  Angle::default_type = Angle::Degrees;

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

