/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/InstallInterpreter.h"
#include "Pulsar/Predictor.h"
#include "Pulsar/Parameters.h"
#include "Pulsar/Receiver.h"
#include "load_factory.h"

using namespace std;

Pulsar::InstallInterpreter::InstallInterpreter ()
{
  add_command 
    ( &InstallInterpreter::parameters,
      "par", "install new pulsar parameters",
      "usage: par <file> \n"
      "  string file  pular parameters filename \n" );

  add_command 
    ( &InstallInterpreter::predictor,
      "pred", "install new pulse phase predictor",
      "usage: pred <file> \n"
      "  string file  pulse phase predictor filename \n" );

  add_command 
    ( &InstallInterpreter::receiver,
      "rcvr", "install new receiver parameters",
      "usage: rcvr <file> \n"
      "  string file  receiver parameters filename \n" );

}

Pulsar::InstallInterpreter::~InstallInterpreter ()
{
}


string Pulsar::InstallInterpreter::parameters (const string& args) try
{ 
  string filename = setup<string> (args);
  get()->set_ephemeris( factory<Parameters> (filename) );
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::InstallInterpreter::predictor (const string& args) try
{ 
  string filename = setup<string> (args);
  get()->set_model( factory<Predictor> (filename) );
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::InstallInterpreter::receiver (const string& args) try
{ 
  string filename = setup<string> (args);
  get()->add_extension( Receiver::load (filename) );
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::InstallInterpreter::empty ()
{
  return response (Fail, help());
}


