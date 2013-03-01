/***************************************************************************
 *
 *   Copyright (C) 2005 - 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ChannelZapMedian.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Statistics.h"

#include "ModifyRestore.h"

#include "median_smooth.h"

using namespace std;

//! Default constructor
Pulsar::ChannelZapMedian::ChannelZapMedian ()
{
  cutoff_threshold = 4.0;
  window_size = 21;
  bybin = false;
  from_total = false;
  paz_report = false;
}

void Pulsar::ChannelZapMedian::operator () (Archive* archive)
{
  if (expression.empty() || expression == "default")
  {
    stats = 0;
    parser = 0;
  }
  else
  {
    if (!stats)
      stats = new Statistics;

    stats->set_Archive( archive );
    stats->set_pol( Index (0,true) );

    parser = stats->get_interface ();
  }

  if (from_total)
  {
    Reference::To<Archive> total = archive->clone ();
    total->pscrunch();
    total->tscrunch();

    weight (total->get_Integration (0));

    unsigned nchan = archive->get_nchan();

#if 0
    for (unsigned ichan=0; ichan < nchan; ichan++)
      cerr << "TOT ZAP " << ichan << " " << single_mask[ichan] << endl;
#endif

    for (unsigned isub = 0; isub < archive->get_nsubint(); isub++)
    {
      Integration* sub = archive->get_Integration (isub);
      for (unsigned ichan=0; ichan < nchan; ichan++)
	if (single_mask[ichan])
	  sub->set_weight (ichan, 0.0);
    }

    return;
  }

  for (unsigned isub = 0; isub < archive->get_nsubint(); isub++)
  {
    if (stats)
      stats->set_subint (isub);

    weight (archive->get_Integration (isub));
  }
}

//! Get the text interface to the configuration attributes
TextInterface::Parser* Pulsar::ChannelZapMedian::get_interface ()
{
  return new Interface (this);
}

Pulsar::ChannelZapMedian::Interface::Interface (ChannelZapMedian* instance)
{
  if (instance)
    set_instance (instance);

  add( &ChannelZapMedian::get_window_size,
       &ChannelZapMedian::set_window_size,
       "window", "Size of median smoothing window" );

  add( &ChannelZapMedian::get_cutoff_threshold,
       &ChannelZapMedian::set_cutoff_threshold,
       "cutoff", "Cutoff threshold" );

  add( &ChannelZapMedian::get_from_total,
       &ChannelZapMedian::set_from_total,
       "total", "Apply a single mask to all integrations" );

  add( &ChannelZapMedian::get_expression,
       &ChannelZapMedian::set_expression,
       "exp", "Statistical expression" );

  add( &ChannelZapMedian::get_bybin,
       &ChannelZapMedian::set_bybin,
       "bybin", "Run algorithm on spectra for each phase bin" );
}

void zap (vector<bool>& mask, vector<float>& spectrum,
	  unsigned window_size, float cutoff_threshold);

//! Set integration weights
void Pulsar::ChannelZapMedian::weight (Integration* integration)
{
  Integration* modify = integration;

  Reference::To<Integration> clone;

  if (bybin && !integration->get_dedispersed())
  {
    /*
      RFI or other instrumental errors may produce a spike in a single
      phase bin.  Using the shift theorem of the Fourier Transform to
      rotate the phase (by a fractional bin) of a profile with a spike
      in it will convert the spike into a sinc function.  Therefore,
      ensure that phase rotations are done in the phase domain
    */
      
    ModifyRestore<bool> change (Profile::rotate_in_phase_domain, true);
    clone = integration->clone();
    clone->dedisperse();
    integration = clone;
  }

  unsigned ichan, nchan = integration->get_nchan ();
  unsigned ipol,  npol = integration->get_npol ();
  unsigned ibin,  nbin = integration->get_nbin ();

  vector<float> spectrum (nchan);
  vector<bool> mask (nchan, false);

  for (ichan=0; ichan < nchan; ichan++)
  {
    if (integration->get_weight(ichan) == 0)
      mask[ichan] = true;

    if (stats)
    {
      stats->set_chan( ichan );
      string value = process( parser, expression );
      spectrum[ichan] = fromstring<float>( value );
    }
    else
    {
      spectrum[ichan] = integration->get_Profile (0, ichan) -> sum();
      if (integration->get_state() == Signal::PPQQ ||
	  integration->get_state() == Signal::Coherence)
	spectrum[ichan] += integration->get_Profile (1, ichan) -> sum();
    }
  }

  zap (mask, spectrum, window_size, cutoff_threshold);

  if (bybin)
  {
    for (ibin=0; ibin < nbin; ibin++)
    {
      for (ichan=0; ichan < nchan; ichan++)
      {
	double polsum = 0;
	for (ipol=0; ipol<npol; ipol++)
        {
	  Profile* profile = integration->get_Profile (ipol, ichan);
	  double temp = profile->get_amps()[ibin];
	  polsum += temp * temp;
	}
	spectrum[ichan] = sqrt(polsum);
      }
      zap (mask, spectrum, window_size, cutoff_threshold);
    }
  }

  unsigned total_zapped = 0;
  for (ichan=0; ichan < nchan; ichan++)
    if (mask[ichan])
    {
      modify->set_weight (ichan, 0.0);
      total_zapped ++;
    }

  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::ChannelZapMedian::weight zapped " << total_zapped
	 << " channels" << endl;

  if (from_total)
    single_mask = mask;

  if (!paz_report)
    return;

  cout << "Equivalent paz cmd: paz -z \"";

  total_zapped = 0;
  for (ichan=0; ichan < nchan; ichan++)
    if (mask[ichan])
    {
      if (total_zapped)
        cout << " ";
      cout << ichan;

      total_zapped ++;
    }

  cout << "\"" << endl;
}

void zap (vector<bool>& mask, vector<float>& spectrum,
	  unsigned window_size, float cutoff_threshold)
{
  vector<float> smoothed_spectrum = spectrum;
    
  fft::median_smooth (smoothed_spectrum, window_size);
    
  double variance = 0.0;
  unsigned total_chan = 0;

  unsigned nchan = spectrum.size();

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
#if 0
    cerr << "ZAP " << ichan
	 << " " << spectrum[ichan]
	 << " " << smoothed_spectrum[ichan] << endl;
#endif

    spectrum[ichan] -= smoothed_spectrum[ichan];
    spectrum[ichan] *= spectrum[ichan];

    if (!mask[ichan])
    {
      variance += spectrum[ichan];
      total_chan ++;
    }
  }

  variance /= total_chan;

  if (Pulsar::Integration::verbose)
    cerr << "Pulsar::ChannelZapMedian::weight variance=" << variance << endl;

  bool zapped = true;
  unsigned round = 1;

  while (zapped)
  {
    float cutoff = cutoff_threshold * cutoff_threshold * variance;

    if (Pulsar::Integration::verbose)
      cerr << "Pulsar::ChannelZapMedian::weight round " << round 
	   << " cutoff=" << cutoff << endl;

    zapped = false;
    round ++;

    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      if (mask[ichan])
	continue;

      if (fabs(spectrum[ichan]) > cutoff)
      {
	// cerr << "cutoff " << ichan << " " << spectrum[ichan] << endl;
	mask[ichan] = true;
      }

      if (ichan && fabs(spectrum[ichan]-spectrum[ichan-1]) > 2*cutoff)
      {
	// cerr << "diff " << ichan << " " << spectrum[ichan] << endl;
	mask[ichan] = true;
      }

      if (mask[ichan])
      {
        variance -= spectrum[ichan]/total_chan;
        zapped = true;
      }
    }
  }
}
