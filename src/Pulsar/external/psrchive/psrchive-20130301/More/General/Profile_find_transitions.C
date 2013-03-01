/***************************************************************************
 *
 *   Copyright (C) 2002 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Profile.h"

using namespace std;

/*! 
  Fractional pulse phase window used to calculate the transitions in
  Pulsar::Profile::find_cal_transition
 */
Pulsar::Option<float>
Pulsar::Profile::transition_duty_cycle
(
 "Profile::transition_duty_cycle", 0.15,

 "Duty cycle used to find transitions [turns]",

 "When searching for transitions between high and low states in a square \n"
 "wave, such as an observation of the reference source (linear noise diode),\n"
 "a sliding window is used to compute the running mean.  The size of this \n"
 "window is given by transition_duty_cycle times the number of bins"
);

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::find_transitions
//
/*! 
  Finds the transitions at which the mean power of a sliding window
  drops below the mean power (high to low) and at which it jumps back
  above the mean power (low to high).  The function is best-suited to
  detect the transitions of a pulsed calibrator profile, which is
  usually a square wave with 0.5 duty cycle.  The width of the sliding
  window is given by Pulsar::Profile::transition_duty_cycle.

  \retval hi2lo bin at which the mean power drops (from left to right)
  \retval lo2hi bin at which the mean power jumps (from left to right)
  \retval width number of bins in the sliding window used to find transitions
*/
void Pulsar::Profile::find_transitions (int& hi2lo, int& lo2hi, int& width)
  const
{
  unsigned nbin = get_nbin();
  const float* amps = get_amps();

  // half the sliding window width
  int buffer = int (0.5 * transition_duty_cycle * float(nbin));

  // the sliding window width
  int box = 2*buffer+1;

  // denominator in mean sliding window power
  double norm = double (box);

  int ibin = 0;
  width = buffer;

  if (verbose)
    cerr << "Pulsar::Profile::find_transitions "
      "nbin=" << nbin << " box=" << box << endl;

  // the average power over the whole profile
  double avg;
  stats (&avg);

  // the running mean in the sliding window
  double running_mean=0;
  for (ibin=-buffer; ibin<=buffer; ++ibin)
    running_mean += amps[(ibin+nbin)%nbin];
  running_mean /= norm;

  if (verbose)
    cerr << "Pulsar::Profile::find_transitions "
      "avg=" << avg << " running_mean=" << running_mean << endl;

  ibin += nbin;
  if (running_mean>avg) {
    while (running_mean>avg) {
      running_mean += (amps[ibin%nbin] - amps[(ibin-box)%nbin])/norm;
      ibin ++;
    }
    hi2lo = (ibin-buffer-1)%nbin;
    while(running_mean<avg){
      running_mean += (amps[ibin%nbin] - amps[(ibin-box)%nbin])/norm;
      ibin ++;
    }
    lo2hi = (ibin-buffer-1)%nbin;
  }
  else {
    while(running_mean<avg){
      running_mean += (amps[ibin%nbin] - amps[(ibin-box)%nbin])/norm;
      ibin ++;
    }
    lo2hi = (ibin-buffer-1)%nbin;
    while(running_mean>avg){
      running_mean += (amps[ibin%nbin] - amps[(ibin-box)%nbin])/norm;
      ibin ++;
    }
    hi2lo = (ibin-buffer-1)%nbin;
  }      

  if (verbose) {
    cerr << "Pulsar::Profile::find_transitions"
      " - high to low transition in bin " << hi2lo << "/"<<nbin << endl
	 << "Pulsar::Profile::find_transitions"
      " - low to high transition in bin " << lo2hi << "/"<<nbin << endl;
  }
}
