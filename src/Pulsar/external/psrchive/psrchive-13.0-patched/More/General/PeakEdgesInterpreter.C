/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PeakEdgesInterpreter.h"
#include "Pulsar/PeakConsecutive.h"
#include "Pulsar/PeakCumulative.h"

#include "Pulsar/Profile.h"

using namespace std;

Pulsar::PeakEdgesInterpreter::PeakEdgesInterpreter ()
{
  add_command 
    ( &PeakEdgesInterpreter::consecutive,
      "consecutive", "install GaussianPeakEdges algorithm",
      "usage: consecutive \n" );

  add_command 
    ( &PeakEdgesInterpreter::cumulative,
      "cumulative", "install PeakEdgesWindow algorithm (default)",
      "usage: cumulative \n" );

}

string Pulsar::PeakEdgesInterpreter::consecutive (const string& args) try
{
  if (!consecutive_functor)
    consecutive_functor.set( new PeakConsecutive, &RiseFall::get_rise_fall );

  Profile::peak_edges_strategy = consecutive_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}

string Pulsar::PeakEdgesInterpreter::cumulative (const string& args) try
{ 
  if (!cumulative_functor)
    cumulative_functor.set( new PeakCumulative, &RiseFall::get_rise_fall );

  Profile::peak_edges_strategy = cumulative_functor;
  return "";
}
catch (Error& error) {
  return error.get_message();
}
    
string Pulsar::PeakEdgesInterpreter::empty ()
{ 
  if (Profile::peak_edges_strategy == consecutive_functor)
    return "consecutive";

  if (Profile::peak_edges_strategy == cumulative_functor)
    return "cumulative";

  return "unknown";
}




