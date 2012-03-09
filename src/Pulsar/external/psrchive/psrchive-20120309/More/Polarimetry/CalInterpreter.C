/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CalInterpreter.h"
#include "Pulsar/CalibratorType.h"

#include "Pulsar/Database.h"
#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/FluxCalibrator.h"
#include "Pulsar/IonosphereCalibrator.h"
#include "Pulsar/FrontendCorrection.h"

using namespace std;

Pulsar::CalInterpreter::CalInterpreter ()
{
  // default calibrator: single axis
  caltype = Calibrator::Type::factory ("single");

  add_command 
    ( &CalInterpreter::type,
      "type", "set or get the calibrator model type",
      "usage: type <name>\n"
      "    string name          name of the calibrator type" );

  add_command
    ( &CalInterpreter::load,
      "load", "load the database or calibrator",
      "usage: load <filename>\n"
      "    string filename      filename of database or calibrator" );

  add_command 
    ( &CalInterpreter::cal,
      "", "calibrate the current archive using the current settings" );

  add_command 
    ( &CalInterpreter::frontend,
      "frontend", "frontend (basis and projection) correction" );
}

Pulsar::CalInterpreter::~CalInterpreter ()
{
}

string Pulsar::CalInterpreter::type (const string& args) try
{
  if (args.empty())
    return response (Good, "type is " + caltype->get_name());

  caltype = Calibrator::Type::factory( args );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, "unrecognized type '" + args + "'");
}

string Pulsar::CalInterpreter::load (const string& args)
{
  string filename = setup<string>(args);
  string dbase_error;
  string cal_error;

  try {
    // try to load the file as a database
    database = new Database( filename );
    calibrator = 0;
    return response (Good);
  }
  catch (Error& error) {
    dbase_error = error.get_message();
  }

  try {
    // try to load the file as a single calibrator
    Reference::To<Archive> archive = Archive::load( filename );
    calibrator = new PolnCalibrator (archive);
    database = 0;
    return response (Good);
  }
  catch (Error& error) {
    cal_error = error.get_message();
  }

  try {
    transformation = MEAL::Function::load<MEAL::Complex2>( filename );
    return response (Good);
  }
  catch (Error& error) {
    return response( Fail,
		     "could not load 1) database 2) calibrator 3) tranform:\n"
		     "1) " + dbase_error + "\n"
		     "2) " + cal_error + "\n"
		     "3) " + error.get_message() );
  }

}

string Pulsar::CalInterpreter::cal (const string& arg) try {

  Reference::To<PolnCalibrator> use_cal;

  if (!calibrator && !database && !transformation)
    return response( Fail, "no database, calibrator or transformation loaded");

  if (transformation)
  {
    Jones<double> xform = transformation->evaluate();
    get()->transform( xform );
  }
  else
  {
    if (calibrator)
      use_cal = calibrator;
    else
      use_cal = database -> generatePolnCalibrator( get(), caltype );
    
    use_cal->calibrate( get() );
  }

  return response (Good);

}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

string Pulsar::CalInterpreter::frontend (const string& args) try
{
  Pulsar::FrontendCorrection correct;
  correct.calibrate( get() );

  return response (Good);
}
catch (Error& error)
{
  return response (Fail, error.get_message());
}

