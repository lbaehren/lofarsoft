/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StatisticsInterface.h"
#include "Pulsar/ProfileStatsInterface.h"
#include "Pulsar/Archive.h"

using namespace std;

Pulsar::Statistics::Interface::Interface (Statistics* instance)
{
  if (instance)
  {
    set_instance (instance);

    if (instance->get_Archive()->get_nsubint() == 0)
      return;
  }

  add( &Statistics::get_subint,
       &Statistics::set_subint,
       "subint", "Sub-integration index" );

  add( &Statistics::get_chan,
       &Statistics::set_chan,
       "chan", "Frequency channel index" );

  add( &Statistics::get_pol,
       &Statistics::set_pol,
       "pol", "Polarization index" );

  if (instance)
  {
    if (Archive::verbose > 2) cerr << "Pulsar::Statistics::Interface::ctor"
      " call Statistics::get_stats" << endl;

    insert ( instance->get_stats()->get_interface() );
  }

  add( &Statistics::get_snr,
       "snr", "Signal-to-noise ratio" );

  add( &Statistics::get_nfnr,
       "nfnr", "Noise-to-Fourier-noise ratio" );

  add( &Statistics::get_cal_ntrans,
       "ncal", "Number of CAL transitions" );

  add( &Statistics::get_2bit_dist,
       "d2bit", "2-bit distortion" );
}


