/***************************************************************************
 *
 *   Copyright (C) 2008 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Statistics.h"
#include "Pulsar/StatisticsInterface.h"

#include "Pulsar/ProfileStats.h"
#include "Pulsar/ProfileShiftFit.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/TwoBitStats.h"
#include "Pulsar/NoiseStatistics.h"
#include "Pulsar/SquareWave.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Integration.h"

using namespace std;


//! Default constructor
Pulsar::Statistics::Statistics (const Archive* data)
{
  set_Archive (data);
}

//! Get the text interface to this
TextInterface::Parser* Pulsar::Statistics::get_interface ()
{
  return new Interface (this);
}

//! Set the instance from which statistics will be drawn
void Pulsar::Statistics::set_Archive (const Archive* data)
{
  archive = data;
  stats_setup = false;
}

const Pulsar::Archive* Pulsar::Statistics::get_Archive () const
{
  return archive;
}

const Pulsar::Integration* Pulsar::Statistics::get_Integration () const
{
  integration = Pulsar::get_Integration (archive, isubint);
  return integration;
}

const Pulsar::Profile* Pulsar::Statistics::get_Profile () const
{
  profile = Pulsar::get_Profile (get_Integration(), ipol, ichan);
  return profile;
}

//! Set the sub-integration from which statistics will be drawn
void Pulsar::Statistics::set_subint (Index _isubint)
{
  isubint = _isubint;
  stats_setup = false;
}

Pulsar::Index Pulsar::Statistics::get_subint () const
{
  return isubint;
}
    
//! Set the frequency channel from which statistics will be drawn
void Pulsar::Statistics::set_chan (Index _ichan)
{
  ichan = _ichan;
  stats_setup = false;
}

Pulsar::Index Pulsar::Statistics::get_chan () const
{
  return ichan;
}

//! Set the polarization to plot
void Pulsar::Statistics::set_pol (Index _ipol)
{
  ipol = _ipol;
  stats_setup = false;
}

Pulsar::Index Pulsar::Statistics::get_pol () const
{
  return ipol;
}

//! Get the signal-to-noise ratio
double Pulsar::Statistics::get_snr () const
{
  return get_Profile()->snr();
}

//! Get the Fourier-noise-to-noise ratio
double Pulsar::Statistics::get_nfnr () const
{
  NoiseStatistics noise;
  if (archive->type_is_cal())
    noise.set_baseline_time (0.4);
  return noise.get_nfnr (get_Profile());
}

double Pulsar::Statistics::get_effective_duty_cycle () const
{
  ProfileShiftFit fit;
  fit.choose_maximum_harmonic = true;
  fit.set_standard( get_Profile() );
  return fit.get_effective_duty_cycle ();
}

//! Get the number of cal transitions
unsigned Pulsar::Statistics::get_cal_ntrans () const
{
  SquareWave wave;
  return wave.count_transitions (get_Profile());
}

//! Get the two bit distortion (or distance from theory)
double Pulsar::Statistics::get_2bit_dist () const
{
  const TwoBitStats* tbs = archive->get<TwoBitStats>();
  if (!tbs)
    return 0;

  double distortion = 0.0;
  unsigned ndig=tbs->get_ndig();
  for (unsigned idig=0; idig < ndig; idig++)
    distortion += tbs->get_distortion(idig);
  return distortion;
}

//! Get the off-pulse baseline
Pulsar::PhaseWeight* Pulsar::Statistics::get_baseline ()
{
  setup_stats ();
  return stats->get_baseline();
}

//! Get the on-pulse phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_onpulse ()
{
  setup_stats ();
  return stats->get_onpulse();
}

//! Get all phase bins
Pulsar::PhaseWeight* Pulsar::Statistics::get_all ()
{
  setup_stats ();
  return stats->get_all();
}

//! Get the statistics calculator
Pulsar::ProfileStats* Pulsar::Statistics::get_stats ()
{
  setup_stats ();
  return stats;
}

//! Get the weighted frequency of the Pulsar::Archive
double Pulsar::Statistics::get_weighted_frequency () const
{
  integration = Pulsar::get_Integration (archive, 0);
  return integration->weighted_frequency (0, archive->get_nchan());
}

void Pulsar::Statistics::setup_stats ()
{
  if (!stats)
  {
    stats = new ProfileStats;
    stats_setup = false;
  }

  if (stats_setup)
    return;

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats stats=" << stats.ptr() << endl;

  stats->set_profile( get_Profile() );

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats profile=" << profile.ptr() << endl;

  // avoid recursion (Plugin::setup might call a function that calls setup_stats)
  stats_setup = true;

  for (unsigned i=0; i<plugins.size(); i++)
    plugins[i]->setup ();

  if (Profile::verbose)
    cerr << "Pulsar::Statistics::setup_stats done" << endl;
}

void Pulsar::Statistics::add_plugin (Plugin* plugin)
{
  plugin->parent = this;
  plugins.push_back(plugin);
  stats_setup = false;
}

