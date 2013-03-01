/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PolnStatistics.h"
#include "Pulsar/PolnProfileStatsInterface.h"
#include "Pulsar/PolnProfile.h"

#include "Pulsar/Integration.h"

using namespace std;

Pulsar::PolnStatistics::PolnStatistics () {}
Pulsar::PolnStatistics::~PolnStatistics () {}

Pulsar::PolnProfileStats* Pulsar::PolnStatistics::get_stats () try
{
  if (!stats)
    stats = new PolnProfileStats;

  /*
    Use the same on-pulse and baseline estimators as used by the 
    parent Statistics class.
  */
  
  ProfileStats* into = stats->get_stats();
  ProfileStats* from = parent->get_stats();

  into->set_onpulse_estimator( from->get_onpulse_estimator() );
  into->set_baseline_estimator( from->get_baseline_estimator() );

  return stats;
}
 catch (Error& error)
   {
     throw error += "Pulsar::PolnStatistics::get_stats";
   }

// this interface wrapper simply postpones the call to get_stats
class Interface : public TextInterface::To<Pulsar::PolnStatistics>
{
public:
  Interface (Pulsar::PolnStatistics* instance)
  {
    if (instance)
      set_instance( instance );

    import ( Pulsar::PolnProfileStats::Interface(), 
	     &Pulsar::PolnStatistics::get_stats );
  }
};

//! Get the text interface to this
TextInterface::Parser* Pulsar::PolnStatistics::get_interface ()
{
  return new Interface (this);
}

void Pulsar::PolnStatistics::setup () try
{
  // cerr << "Pulsar::PolnStatistics::setup this=" << this << endl;

  profile = Pulsar::get_Stokes (get_Integration(), parent->get_chan());

  PhaseWeight* baseline = parent->get_stats()->get_baseline();

  Reference::To<PolnProfile> p = profile->clone();
  for (unsigned i=0; i<4; i++)
  {
    Profile* s = p->get_Profile(i);
    baseline->set_Profile(s);
    s->offset( -baseline->get_mean().get_value() );
  }
  profile = p;

  get_stats()->set_profile( profile );
}
 catch (Error& error)
   {
     throw error += "Pulsar::PolnStatistics::setup";
   }
