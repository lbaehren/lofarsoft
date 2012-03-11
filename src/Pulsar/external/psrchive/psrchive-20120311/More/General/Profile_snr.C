/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"
#include "Pulsar/SNRatioInterpreter.h"

#include <math.h>

using namespace Pulsar;
using namespace std;

/*!  The SNRatioInterpreter class sets the snr_strategy attribute
  according to commands specified either in the configuration file or
  via the psrsh interpreter.  It enables convenient experimentation
  with the S/N estimation algorithm.
*/
Pulsar::Option<Profile::Float> Pulsar::Profile::snr_strategy
(
 new Pulsar::SNRatioInterpreter,
 "Profile::snr", "phase",

 "Algorithm used to compute S/N",

 "The name of the algorithm used to estimate the signal-to-noise ratio\n"
 "of the pulse profile. Possible values: phase, fourier, square, adaptive, \n"
 "and standard <filename.ar>"
);

/*! This method calls Profile::snr_strategy */
float Pulsar::Profile::snr() const try {
  
  if (verbose)
    cerr << "Pulsar::Profile::snr" << endl;
  
  return snr_strategy.get_value() (this);
  
}
catch (Error& error) {
  throw error += "Pulsar::Profile::snr";
}

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

/*! This default implementation of the Profile::snr method is taken
 from the old timer archive code.  Using Profile::find_min_phase and
 Profile::find_peak_edges, this function finds the integrated power in
 the pulse profile and divides this by the noise in the baseline.
*/
double Pulsar::snr_phase (const Profile* profile)
{
  // find the mean and the r.m.s. of the baseline
  double min_avg, min_var;
  profile->stats (profile->find_min_phase(), &min_avg, &min_var);
  double min_rms = sqrt (min_var);

  if (Profile::verbose)
    cerr << "Pulsar::snr_phase rms=" << min_rms << endl;

  if (min_rms == 0.0)
    return 0;

  // find the total power under the pulse
  int rise = 0, fall = 0;
  profile->find_peak_edges (rise, fall);

  double power = profile->sum (rise, fall);

  nbinify (rise, fall, profile->get_nbin());

  if (Profile::verbose)
    cerr << "Pulsar::snr_phase rise=" << rise << " fall=" << fall 
	 << " power=" << power << endl;

  // subtract the total power due to the baseline
  power -= min_avg * double (fall - rise);

  // divide by the sqrt of the number of bins
  power /= sqrt (double(fall-rise));

  return power/min_rms;
}
