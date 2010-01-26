/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;

#include "Pulsar/NoiseStatistics.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Fourier.h"

#include <math.h>

#if defined(sun)
#include <ieeefp.h>
#endif

Pulsar::NoiseStatistics::NoiseStatistics ()
{
  baseline_fourier = 0.45;
  baseline_time = Profile::default_duty_cycle;
}

//! Set the fractional number of high frequencies used to calculate noise
void Pulsar::NoiseStatistics::set_baseline_fourier (float extent)
{
  if (extent < 0 || extent > 1.0)
    throw Error (InvalidParam, "Pulsar::NoiseStatistics::set_baseline_fourier",
		 "invalid extent = %f", extent);

  baseline_fourier = extent;
}

//! Set the fractional number of high frequencies used to calculate noise
void Pulsar::NoiseStatistics::set_baseline_time (float extent)
{
  if (extent < 0 || extent > 1.0)
    throw Error (InvalidParam, "Pulsar::NoiseStatistics::set_baseline_time",
		 "invalid extent = %f", extent);

  baseline_time = extent;
}

//! Return the noise to Fourier noise ratio
float Pulsar::NoiseStatistics::get_nfnr (const Profile* profile)
{
  Reference::To<Profile> fourier = fourier_transform (profile);
  detect (fourier);

  unsigned nbin = fourier->get_nbin();
  float* amps = fourier->get_amps();

  // note that DC and nyquist terms are thrown out in the following loop
  unsigned start = unsigned (nbin * (1.0 - baseline_fourier));

  unsigned count = 0;
  double total = 0.0;

  for (unsigned ibin=start; ibin < nbin; ibin++) {
    if (!finite(amps[ibin]))
      throw Error (InvalidState, "Pulsar::NoiseStatistics::get_nfnr",
		   "amps[%d] = %f", ibin, amps[ibin]);
    total += amps[ibin];
    count ++;
  }

  double f_var = total/count;

  float phase = profile->find_min_phase (baseline_time);

  double t_var = 0;
  profile->stats (phase, 0, &t_var, 0, baseline_time);

  double nfnr = sqrt(t_var/f_var);

  if (Profile::verbose)
    cerr << "Pulsar::NoiseStatistics::get_nfnr \n"
      "  t_var=" << t_var << " f_var=" << f_var << " nfnr=" << nfnr << endl;

  return nfnr;
}    
