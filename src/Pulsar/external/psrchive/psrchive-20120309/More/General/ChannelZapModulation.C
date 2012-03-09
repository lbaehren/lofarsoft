/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include "Pulsar/ChannelZapModulation.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

//! Default constructor
Pulsar::ChannelZapModulation::ChannelZapModulation ()
{
  cutoff_threshold = 4.0;
}

//! Set integration weights
void Pulsar::ChannelZapModulation::weight (Integration* integration)
{
  unsigned ichan, nchan = integration->get_nchan ();

  vector<float> spectrum (nchan);

  for (ichan=0; ichan < nchan; ichan++) {

    Profile* profile = integration->get_Profile (0, ichan);

    double mean, variance;
    profile->stats (&mean, &variance);

    if (integration->get_state() == Signal::PPQQ ||
	integration->get_state() == Signal::Coherence) {

      profile = integration->get_Profile (1, ichan);
      double mean2, variance2;
      profile->stats (&mean2, &variance2);

      mean += mean2;
      variance += variance2;

    }

    spectrum[ichan] = sqrt(variance) / mean;
    
    cerr << ichan << ": " << spectrum[ichan] << endl;

  }

#if 0
  for (ichan=0; ichan < nchan; ichan++)
    integration->set_weight (ichan, 0.0);
#endif

}
