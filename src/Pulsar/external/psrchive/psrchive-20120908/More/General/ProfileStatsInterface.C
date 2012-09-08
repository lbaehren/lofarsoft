/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatsInterface.h"
#include "Pulsar/PhaseWeightInterface.h"

#include "Pulsar/ProfileWeightFunction.h"
#include "interface_stream.h"

using namespace std;


std::ostream& operator<< (std::ostream& ostr,
			  Pulsar::ProfileWeightFunction* e)
{
  return interface_insertion (ostr, e);
}
std::istream& operator>> (std::istream& istr,
			  Pulsar::ProfileWeightFunction* &e)
{
  return interface_extraction (istr, e);
}


Pulsar::ProfileStats::Interface::Interface (ProfileStats* instance)
{
  if (instance)
    set_instance (instance);

  // cerr << "Pulsar::ProfileStats::Interface constructor" << endl;

  typedef PhaseWeight* (ProfileStats::*Method) (void);

  add( &ProfileStats::get_onpulse_estimator,
       &ProfileStats::set_onpulse_estimator,
       "on", "On-pulse estimator" );

  import( "on", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_onpulse );

  add( &ProfileStats::get_baseline_estimator,
       &ProfileStats::set_baseline_estimator,
       "off", "Off-pulse estimator" );

  import( "off", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_baseline );

  import( "all", PhaseWeight::Interface(), 
	  (Method) &ProfileStats::get_all );
}


