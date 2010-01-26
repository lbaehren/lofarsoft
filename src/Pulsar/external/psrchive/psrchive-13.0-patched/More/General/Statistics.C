/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Statistics.h"
#include "Pulsar/ProfileStats.h"

#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/TwoBitStats.h"
#include "Pulsar/NoiseStatistics.h"
#include "Pulsar/SquareWave.h"
#include "Pulsar/Profile.h"

using namespace std;


//! Default constructor
Pulsar::Statistics::Statistics (const Archive* data)
{
  if (data)
    set_Archive (data);
}

//! Set the instance from which statistics will be drawn
void Pulsar::Statistics::set_Archive (const Archive* data)
{
  archive = data;
}

const Pulsar::Archive* Pulsar::Statistics::get_Archive () const
{
  return archive;
}

const Pulsar::Profile* Pulsar::Statistics::get_Profile () const
{
  profile = Pulsar::get_Profile (archive, isubint, ipol, ichan);
  return profile;
}

//! Set the sub-integration from which statistics will be drawn
void Pulsar::Statistics::set_subint (Index _isubint)
{
  isubint = _isubint;
  setup_stats ();
}

Pulsar::Index Pulsar::Statistics::get_subint () const
{
  return isubint;
}
    
//! Set the frequency channel from which statistics will be drawn
void Pulsar::Statistics::set_chan (Index _ichan)
{
  ichan = _ichan;
  setup_stats ();
}

Pulsar::Index Pulsar::Statistics::get_chan () const
{
  return ichan;
}

//! Set the polarization to plot
void Pulsar::Statistics::set_pol (Index _ipol)
{
  ipol = _ipol;
  setup_stats ();
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

void Pulsar::Statistics::setup_stats ()
{
  if (!stats)
    stats = new ProfileStats;

  stats->set_profile( get_Profile() );
}
