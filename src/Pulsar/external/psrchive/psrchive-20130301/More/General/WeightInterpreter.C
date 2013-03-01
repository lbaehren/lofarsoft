/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/WeightInterpreter.h"
#include "Pulsar/RadiometerWeight.h"
#include "Pulsar/DurationWeight.h"
#include "Pulsar/SNRWeight.h"

using namespace std;

Pulsar::WeightInterpreter::WeightInterpreter (Policy& _policy)
  : policy (_policy)
{
  add_command 
    ( &WeightInterpreter::radiometer,
      "radiometer", "install RadiometerWeight algorithm",
      "usage: radiometer \n" );

  add_command 
    ( &WeightInterpreter::time,
      "time", "install DurationWeight algorithm",
      "usage: time \n" );

  add_command 
    ( &WeightInterpreter::snr,
      "snr", "install SNRWeight algorithm",
      "usage: snr \n" );

  add_command 
    ( &WeightInterpreter::none,
      "none", "disabling weighting",
      "usage: none \n" );
}

string Pulsar::WeightInterpreter::radiometer (const string& args) try
{
  if (!radiometer_functor)
    radiometer_functor.set( new RadiometerWeight, &RadiometerWeight::weight );

  policy = radiometer_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::WeightInterpreter::time (const string& args) try
{
  if (!time_functor)
    time_functor.set( new DurationWeight, &DurationWeight::weight );

  policy = time_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::WeightInterpreter::snr (const string& args) try
{
  if (!snr_functor)
    snr_functor.set( new SNRWeight, &SNRWeight::weight );

  policy = snr_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::WeightInterpreter::none (const string& args)
{
  Policy none;
  policy = none;
  return "";
}
    
string Pulsar::WeightInterpreter::empty ()
{ 
  if (policy == radiometer_functor)
    return "radiometer";

  if (policy == time_functor)
    return "time";

  if (policy == snr_functor)
    return "snr";

  if (!policy)
    return "none";

  return "unknown";
}

