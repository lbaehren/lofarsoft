/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "find_spike_edges.h"
#include "Pulsar/Profile.h"

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_spike_edges
//
/*! Works out where the instantaneous flux drops to \param pc percent of
the flux in \param spike_bin.  This routine is designed for use of single
pulses where the duty cycle and hence integrated flux is low.
 */
void Pulsar::find_spike_edges(const Profile* profile, int& rise, int& fall,
                              float pc, int spike_bin)
{
  if( spike_bin < 0 )
    spike_bin = profile->find_max_bin();

  int _nbin = profile->get_nbin();

  if( spike_bin < int(0) || spike_bin >= _nbin )
    throw Error(InvalidParam,"Pulsar::Profile::find_spike_edges()",
		"spike_bin=%d not a valid bin- must be in range [0,%d)",
		spike_bin, _nbin);

  const float* amps = profile->get_amps();

  float mean_level = profile->mean(profile->find_min_phase());
  float relative_amp = amps[spike_bin] - mean_level;
  float threshold = pc*relative_amp + mean_level;

  bool found_spike_edge = false;

  /////////////////////////////////////////////////
  // Find where spike begins
  for( int irise=spike_bin-1; irise>spike_bin-_nbin; irise--){
    int jrise = irise;
    if( jrise < 0 )
      jrise += _nbin;

    if( amps[jrise] < threshold ){
      found_spike_edge = true;
      rise = jrise+1;
      break;
    }
  }

  if( !found_spike_edge )
    throw Error(InvalidState,"Pulsar::Profile::find_spike_edges()",
		"Could not find spike edge for rise dropoff to %f of %f = %f.  Minimum=%f maximum=%f",
		pc, amps[spike_bin], threshold,
		profile->min(), profile->max());

  found_spike_edge = false;

  /////////////////////////////////////////////////
  // Find where spike ends
  for( int ifall=spike_bin+1; ifall<spike_bin+_nbin; ifall++){
    int jfall = ifall;
    if( jfall > _nbin )
      jfall -= _nbin;

    if( amps[jfall] < threshold ){
      found_spike_edge = true;
      fall = jfall;
      break;
    }
  }

  if( !found_spike_edge )
    throw Error(InvalidState,"Pulsar::Profile::find_spike_edges()",
		"Could not find spike edge for fall dropoff to %f of %f = %f.  Minimum=%f maximum=%f",
		pc, amps[spike_bin], threshold,
		profile->min(), profile->max());
}

