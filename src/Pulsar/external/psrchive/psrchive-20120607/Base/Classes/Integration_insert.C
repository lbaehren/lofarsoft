/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include <algorithm>

using namespace std;

bool increasing_frequency (const Reference::To<Pulsar::Profile>& a, 
			   const Reference::To<Pulsar::Profile>& b)
{
  return a->get_centre_frequency() < b->get_centre_frequency();
}

bool decreasing_frequency (const Reference::To<Pulsar::Profile>& a, 
			   const Reference::To<Pulsar::Profile>& b)
{
  return b->get_centre_frequency() < a->get_centre_frequency();
}

void Pulsar::Integration::insert (Integration* from)
try
{
  unsigned npol  = get_npol();
  unsigned nchan = get_nchan();
  unsigned nbin  = get_nbin();

  if (from->get_npol() != npol)
    throw Error (InvalidState, "Pulsar::Integration::insert",
		 "Integrations have different numbers of polarizations:"
		 " %u != %u", get_npol(), from->get_npol());

  if (from->get_nbin() != nbin)
    throw Error (InvalidState, "Pulsar::Integration::insert",
		 "Integrations have different numbers of phase bins:"
		 " %u != %u", get_nbin(), from->get_nbin());

  unsigned new_nchan = nchan + from->get_nchan();
  double bandwidth = get_bandwidth();

  for (unsigned ipol=0; ipol < npol; ipol++) {

    profiles[ipol].resize (new_nchan);

    for (unsigned ichan=0; ichan < from->get_nchan(); ichan++)
      profiles[ipol][nchan+ichan] = from->get_Profile (ipol, ichan);

    std::sort (profiles[ipol].begin(), profiles[ipol].end(),
	       (bandwidth > 0) ? increasing_frequency : decreasing_frequency);

  }

  set_nchan (new_nchan);

}
catch (Error& error)
{
  throw error += "Pulsar::Integration::insert";
}

