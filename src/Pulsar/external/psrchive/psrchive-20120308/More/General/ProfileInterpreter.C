/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileInterpreter.h"
#include "Pulsar/ArchiveTemplates.h"

#include "Pulsar/Accumulate.h"
#include "Pulsar/Differentiate.h"
#include "Pulsar/SmoothMean.h"
#include "Pulsar/SmoothMedian.h"
#include "Pulsar/SmoothSinc.h"
#include "Pulsar/Subtract.h"

using namespace std;

string help_smooth (const string& method)
{
  return
    "usage: " + method + " <factor> \n"
    "where: <factor> is the width of the smoothing function \n"
    "       if factor < 1, then it is interpreted as turns \n"
    "       if factor > 1, then it is interpreted as phase bins \n";
}

Pulsar::ProfileInterpreter::ProfileInterpreter ()
{
  add_command 
    ( &ProfileInterpreter::baseline,
      "baseline", "remove the profile baseline",
      "usage: baseline \n" );

  add_command 
    ( &ProfileInterpreter::mean,
      "mean", "form the mean smoothed profile",
      help_smooth ("mean") );

  add_command 
    ( &ProfileInterpreter::median,
      "median", "form the median smoothed profile",
      help_smooth ("median") );

  add_command 
    ( &ProfileInterpreter::sinc,
      "sinc", "form the low-pass filtered profile",
      help_smooth ("sinc") );

  add_command 
    ( &ProfileInterpreter::cumulative,
      "cumulative", "form the cumulative profile",
      "usage: cumulative\n" );

  add_command 
    ( &ProfileInterpreter::difference,
      "difference", "form the difference profile",
      "usage: difference \n" );

  add_command 
    ( &ProfileInterpreter::subtract,
      "subtract", "subtract profiles from the named archive",
      "usage: subtract <name>\n" );
}

Pulsar::ProfileInterpreter::~ProfileInterpreter ()
{
}

string Pulsar::ProfileInterpreter::baseline (const string& args) try
{
  get()->remove_baseline();
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::cumulative (const string& args) try
{
  foreach (get(), new Accumulate);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::difference (const string& args) try
{ 
  foreach (get(), new Differentiate);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::mean (const string& args)
{ 
  return smooth (new SmoothMean, args);
}

string Pulsar::ProfileInterpreter::median (const string& args)
{ 
  return smooth (new SmoothMedian, args);
}

string Pulsar::ProfileInterpreter::sinc (const string& args)
{ 
  return smooth (new SmoothSinc, args);
}

string Pulsar::ProfileInterpreter::smooth (Smooth* smooth, const string& args)
try {

  Reference::To<Smooth> smooth_ref = smooth;

  float factor = setup<float> (args);

  if (factor < 1.0)
    smooth->set_turns (factor);
  else
    smooth->set_bins (factor);

  foreach( get(), smooth );
  return response (Good);

}
catch (Error& error) {
  return response (Fail, error.get_message());
}

string Pulsar::ProfileInterpreter::subtract (const string& args) try
{ 
  string name = setup<string> (args);
  foreach (get(), getmap(name), new Subtract);
  return response (Good);
}
catch (Error& error) {
  return response (Fail, error.get_message());
}


string Pulsar::ProfileInterpreter::empty ()
{
  return response (Fail, help());
}


