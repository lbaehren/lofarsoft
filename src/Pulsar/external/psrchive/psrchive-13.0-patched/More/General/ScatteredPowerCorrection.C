/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ScatteredPowerCorrection.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "Physical.h"

using namespace std;

//! Default constructor
Pulsar::ScatteredPowerCorrection::ScatteredPowerCorrection ()
{
}

void Pulsar::ScatteredPowerCorrection::correct (Archive* data)
{
  twobit_stats = data->get<TwoBitStats>();
  if (twobit_stats)
    ja98_a5.set_nsamp (twobit_stats->get_nsample());
  else
    ja98_a5.set_nsamp (512);

  twobit_stats = 0;

  for (unsigned isub=0; isub < data->get_nsubint(); isub++)
    transform (data->get_Integration(isub));
}

void Pulsar::ScatteredPowerCorrection::transform (Integration* data)
{
  unsigned nchan = data->get_nchan();
  unsigned nbin  = data->get_nbin();
  unsigned npol  = data->get_npol();

  if (npol == 4)
  {
    if (data->get_state() != Signal::Coherence)
      throw Error (InvalidParam, "Pulsar::ScatteredPowerCorrection::transform",
		   "data with npol==4 must have state=Signal::Coherence");
    npol = 2;
  }

  // check that scattered power is not significantly dispersed
  double chan_bw = data->get_bandwidth() / nchan;
  double DM = data->get_dispersion_measure();
  double time_resolution = data->get_folding_period() / nbin;

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    double c_freq= data->get_centre_frequency(ichan);
    double time_smear = dispersion_smear (DM, c_freq, chan_bw);
    if (time_smear > time_resolution)
      throw Error (InvalidParam, "Pulsar::ScatteredPowerCorrection::transform",
		   "smearing in ichan=%u = %lf > time resolution = %lf",
		   ichan, time_smear, time_resolution);
  }

  if (Profile::verbose)
    cerr << "Pulsar::ScatteredPowerCorrection::transform results" << endl;

  for (unsigned ipol = 0; ipol < npol; ipol++)
  {
    if (twobit_stats)
      ja98_a5.set_A6( twobit_stats->get_histogram(ipol) );

    for (unsigned ibin = 0; ibin < nbin; ibin++)
    {
      unsigned ichan = 0;
      double power = 0.0;
      
      for (ichan = 0; ichan < nchan; ichan++)
      {
	// scattered power correction requires all channels
	if (data->get_weight(ichan) == 0.0)
	  throw Error (InvalidParam,
		       "Pulsar::ScatteredPowerCorrection::transform",
		       "ichan=%u has zero weight", ipol, ichan);

	power += data->get_Profile(ipol,ichan)->get_amps()[ibin];
      }

      power /= nchan;

      // compute the quantization noise power in each channel

      double Phi = ja98_a5.invert( power );
      ja98.set_Phi ( Phi );

      double A = ja98.get_A();
      double scattered_power = power * (1-A);

      if (Profile::verbose)
	cerr << "  ipol=" << ipol << " ibin=" << ibin << " power=" << power
	     << " Phi=" << Phi << " A=" << A << endl;

      for (ichan = 0; ichan < nchan; ichan++)
	data->get_Profile(ipol,ichan)->get_amps()[ibin] -= scattered_power;
    }
  }
}
