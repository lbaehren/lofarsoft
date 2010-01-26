/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PeakCumulative.h"

#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Accumulate.h"
#include "Pulsar/Profile.h"

#include <iostream>

using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::PeakCumulative::PeakCumulative ()
{
  threshold = 0.1; // 10% of total

  bin_start = bin_end = 0;
  range_specified = false;

  bin_rise = bin_fall = 0;
  choose = true;

  built = false;
}

Pulsar::PeakCumulative* Pulsar::PeakCumulative::clone () const
{
  return new PeakCumulative (*this);
}

void Pulsar::PeakCumulative::set_Profile (const Profile* p)
{
  profile = p;
  built = false;
}

//! Set the duty cycle
void Pulsar::PeakCumulative::set_threshold (float _threshold)
{
  if (threshold != _threshold)
    built = false;
  threshold = _threshold;
}

float Pulsar::PeakCumulative::get_threshold () const
{
  return threshold;
}

//! Set the choose
void Pulsar::PeakCumulative::set_choose (bool c)
{
  if (choose != c)
    built = false;
  choose = c;
}

//! Get the choose
bool Pulsar::PeakCumulative::get_choose () const
{
  return choose;
}

//! Set the BaselineEstimator used to find the off-pulse phase bins
void Pulsar::PeakCumulative::set_baseline_estimator (BaselineEstimator* b)
{
  baseline_estimator = b;
  built = false;
}

//! Get the BaselineEstimator used to find the off-pulse phase bins
const Pulsar::BaselineEstimator* 
Pulsar::PeakCumulative::get_baseline_estimator () const
{
  return baseline_estimator;
}

Pulsar::BaselineEstimator*
Pulsar::PeakCumulative::get_baseline_estimator ()
{
  return baseline_estimator;
}

//! Set the start and end bins of the search
void Pulsar::PeakCumulative::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
  built = false;
}

void Pulsar::PeakCumulative::build ()
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PeakCumulative::compute",
		 "Profile not set");

  Reference::To<PhaseWeight> baseline;

  if (baseline_estimator)
    baseline = baseline_estimator->baseline (profile);
  else
    baseline = profile->baseline();

  Estimate<double> mean = baseline->get_mean ();
  Estimate<double> var = baseline->get_variance ();
  Estimate<double> rms = sqrt(var);

  if (Profile::verbose)
    cerr << "Pulsar::PeakCumulative::compute baseline"
      " mean=" << mean << " rms=" << rms << endl;

  unsigned nbin = profile->get_nbin();

  int start = 0;
  int stop = nbin;

  if (range_specified)
  {
    start = bin_start;
    stop  = bin_end;
    nbinify (start, stop, nbin);
  }

  // the total power under the pulse
  double tot_amp = profile->sum() - mean.get_value() * double(nbin);

  if (Profile::verbose)
    cerr << "Pulsar::PeakCumulative::compute baseline mean=" << mean
	 << " total power=" << tot_amp << " threshold=" << threshold << endl;

  // set the thresholds for the search
  double rise_threshold = double(threshold) * tot_amp;
  double fall_threshold = double(1.0-threshold) * tot_amp;

  // start on first bin, then on nbin/2
  unsigned istart = 0;

  // one result for each try
  int irise[2];
  int ifall[2];  

  unsigned ntries = 2;

  if (!choose)
    ntries = 1;

  for (unsigned itry=0; itry<ntries; itry++)
  {
    // space to hold the cumulative sum
    Profile cumulative (*profile);

    Accumulate accumulate;
    accumulate.set_offset (istart);
    accumulate.set_baseline (mean.get_value());
    accumulate.transform (&cumulative);

    float* cumu = cumulative.get_amps();

    // find where cumulative sum falls below min_threshold for the last time
    irise[itry] = 0;

    int ibin = 0;
    for (ibin=start; ibin<stop; ibin++)
      if (cumu[ibin] < rise_threshold) 
	irise[itry] = ibin;

    //    cpgeras();
    //cpgsci(1);
    //cpgswin(0.0,(float)nbin,cumu[0],cumu[nbin-1]);
    //cpgmove(0.0,cumu[0]);
    //for (ibin=0;ibin<nbin;ibin++){
    //  cpgdraw((float)ibin,cumu[ibin]);
    //}
    //cpgsci(2);
    //cpgmove(0.0,rise_threshold);
    //cpgdraw((float)nbin-1,rise_threshold);
    //cpgsci(2);
    //cpgmove(0.0,fall_threshold);
    //cpgdraw((float)nbin-1,fall_threshold);

    // find where cumulative sum falls below max_threshold for the first time.
    ifall[itry] = nbin-1;
    for (ibin=stop-1; ibin>=start; ibin--)
      if (cumu[ibin] > fall_threshold) 
	ifall[itry] = ibin;
    
    if (Profile::verbose)
      cerr << "Pulsar::PeakCumulative::compute try=" << itry 
	   << " irise=" << irise[itry] << " ifall=" << ifall[itry] << endl;

    // do it again; this time starting half way along.
    istart = nbin/2;

  }

  // Decide which of the searches is better - ie. provides a narrower pulse.
  int diff0 = ifall[0] - irise[0];
  int diff1 = ifall[1] - irise[1];

  if ( !choose || (diff0 < diff1 && diff0 > 0) ) {
    bin_rise = irise[0];
    bin_fall = ifall[0];
  }
  else if (diff1 > 0) {
    bin_rise = irise[1] + nbin/2;
    bin_fall = ifall[1] + nbin/2;
  }
  else {
    // In noisy data, ifall can be < irise!  In this case, just find
    // the maximum and return with pulse width = 0.4
    float phase = profile->find_max_phase (.4);
    bin_rise = int ((phase - 0.2) * nbin);
    bin_fall = int ((phase + 0.2) * nbin);
  }

  bin_rise %= nbin;
  bin_fall %= nbin;

}


/*!
  \retval rise bin at which the cumulative power last remains above threshold
  \retval fall bin at which the cumulative power first falls below threshold
*/
void Pulsar::PeakCumulative::get_indeces (int& rise, int& fall) const
{
  if (!built)
    const_cast<PeakCumulative*>(this)->build ();

  rise = bin_rise;
  fall = bin_fall;
}

class Pulsar::PeakCumulative::Interface 
  : public TextInterface::To<PeakCumulative>
{
public:
  Interface (PeakCumulative* instance)
  {
    if (instance)
      set_instance (instance);

    add( &PeakCumulative::get_threshold,
	 &PeakCumulative::set_threshold,
	 "threshold", "fractional cumulative power at edge" );
  }

  std::string get_interface_name () const { return "cumulative"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PeakCumulative::get_interface ()
{
  return new Interface (this);
}
