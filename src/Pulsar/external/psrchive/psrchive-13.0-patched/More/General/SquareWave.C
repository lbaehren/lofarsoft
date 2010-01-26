/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SquareWave.h"
#include "Pulsar/BaselineWindow.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Smooth.h"

using namespace std;

Pulsar::SquareWave::SquareWave ()
{
  risetime = 0.03;
  threshold = 6.0;
  use_nbin = 256;
}

//! Return the signal to noise ratio
float Pulsar::SquareWave::get_snr (const Profile* profile)
{
  int hightolow, lowtohigh, buffer;
  profile->find_transitions (hightolow, lowtohigh, buffer);

  double hi_mean, hi_var;
  profile->stats (&hi_mean, &hi_var, 0,
		  lowtohigh + buffer,
		  hightolow - buffer);

  double lo_mean, lo_var;
  profile->stats (&lo_mean, &lo_var, 0,
		  hightolow + buffer,
		  lowtohigh - buffer);

  // two unique estimates of the variance -> take the average
  double var = (lo_var + hi_var) * 0.5;

  return (hi_mean - lo_mean) / sqrt(var);
}    

Pulsar::Profile* differentiate (const Pulsar::Profile* profile, unsigned off=1)
{
  Reference::To<Pulsar::Profile> difference = profile->clone();

  unsigned nbin = profile->get_nbin();
  const float* amps = profile->get_amps();
  float* damps = difference->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++)
    damps[ibin] = amps[(ibin+off)%nbin] - amps[ibin];

  return difference.release();
}

// find the transitions
void find_transitions (unsigned nbin, float* amps, vector<unsigned>& t,
		       float cutoff)
{
  for (unsigned ibin=0; ibin<nbin; ibin++) {

    bool passover = false;
    unsigned stbin = 0;

    if ( (cutoff > 0 && amps[ibin] > cutoff) || 
	 (cutoff < 0 && amps[ibin] < cutoff) ) {
      stbin = ibin;
      passover = true;
    }

    if (passover) {
      // try to avoid counting the same transition twice by searching
      // for three samples in a row with difference less than cutoff
      unsigned count = 0;
      while ( ibin < nbin ) {
	if (fabs(amps[ibin]) < fabs(cutoff))
	  count ++;
	else
	  count = 0;
	if (count > 2)
	  break;
	ibin++;
      }

      t.push_back ((stbin+ibin-3)/2);
    }
    
  }

}

void Pulsar::SquareWave::get_transitions (const Profile* profile,
					  vector<unsigned>& up,
					  vector<unsigned>& down)
{
  unsigned nbin = profile->get_nbin();
  unsigned scrunch = 1;

  Reference::To<Profile> clone;
  if (use_nbin && nbin > use_nbin) {
    clone = profile->clone();
    scrunch = nbin/use_nbin;
    clone->bscrunch(scrunch);
    profile = clone;
    nbin = profile->get_nbin();
  }

  unsigned offset = (unsigned) (risetime * nbin);

cerr << "nbin=" << nbin << " offset=" << offset << endl;

  // differentiate the profile
  Reference::To<Profile> difference = differentiate (profile, offset);

  float* amps = difference->get_amps();

  // find the phase window in which the mean is closest to zero
  BaselineWindow window;
  window.set_find_mean (0.0);
  window.get_smooth()->set_turns (0.2);
  float zero = window.find_phase (nbin, amps);

  // get the noise statistics of the zero mean region
  double mean = 0;
  double variance = 0;
  difference->stats (zero, &mean, &variance);

cerr << "mean=" << mean << " rms=" << sqrt(variance) << endl;

  // check that the mean is actually zero
  double rms = sqrt(variance);
  if (mean > rms)
    throw Error (InvalidState, "Pulsar::SquareWave::get_transitions",
		 "mean=%lf > rms=%lf", mean, rms);

  float cutoff = threshold * rms;

  find_transitions (nbin, amps, up, cutoff);
  find_transitions (nbin, amps, down, -cutoff);
}

unsigned Pulsar::SquareWave::count_transitions (const Profile* profile)
{
  vector<unsigned> up;
  vector<unsigned> down;
  get_transitions (profile, up, down);

#if 0
  cerr << "UP=" << lowtohigh << " DOWN=" << hightolow << endl;

  cerr << "Found " << up.size() << " up & " << down.size() << " down" << endl;
  for (unsigned i=0; i<up.size(); i++)
    cerr << "up " << up[i] << endl;
  for (unsigned i=0; i<down.size(); i++)
    cerr << "down " << down[i] << endl;
#endif

  return std::min( up.size(), down.size() );
}
