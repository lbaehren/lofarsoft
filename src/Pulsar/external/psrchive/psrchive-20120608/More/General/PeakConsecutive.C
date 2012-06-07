/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PeakConsecutive.h"

#include "Pulsar/BaselineEstimator.h"
#include "Pulsar/PhaseWeight.h"

#include "Pulsar/Accumulate.h"
#include "Pulsar/Profile.h"

#include <iostream>
#include <assert.h>

using namespace std;

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

//! Default constructor
Pulsar::PeakConsecutive::PeakConsecutive ()
{
  threshold = 3.0;  // 3 sigma
  consecutive = 3;  // pretty unlikely by chance

  bin_start = bin_end = 0;
  range_specified = false;

  bin_rise = bin_fall = 0;

  merge_regions = false;
  built = false;
}

Pulsar::PeakConsecutive* Pulsar::PeakConsecutive::clone () const
{
  return new PeakConsecutive (*this);
}

void Pulsar::PeakConsecutive::set_Profile (const Profile* p)
{
  profile = p;
  built = false;
}

//! Set the duty cycle
void Pulsar::PeakConsecutive::set_threshold (float _threshold)
{
  if (threshold != _threshold)
    built = false;
  threshold = _threshold;
}

float Pulsar::PeakConsecutive::get_threshold () const
{
  return threshold;
}

//! Set the consecutive
void Pulsar::PeakConsecutive::set_consecutive (unsigned c)
{
  if (consecutive != c)
    built = false;
  consecutive = c;
}

//! Get the consecutive
unsigned Pulsar::PeakConsecutive::get_consecutive () const
{
  return consecutive;
}

//! Set the BaselineEstimator used to find the off-pulse phase bins
void Pulsar::PeakConsecutive::set_baseline_estimator (BaselineEstimator* b)
{
  baseline_estimator = b;
  built = false;
}

//! Get the BaselineEstimator used to find the off-pulse phase bins
const Pulsar::BaselineEstimator* 
Pulsar::PeakConsecutive::get_baseline_estimator () const
{
  return baseline_estimator;
}

Pulsar::BaselineEstimator*
Pulsar::PeakConsecutive::get_baseline_estimator ()
{
  return baseline_estimator;
}

//! Set the start and end bins of the search
void Pulsar::PeakConsecutive::set_range (int start, int end)
{
  bin_start = start;
  bin_end = end;
  range_specified = true;
  built = false;
}

template<typename C>
void cyclic_shift (vector<C>& array)
{
  C temp = array[0];
  for (unsigned i=1; i<array.size(); i++)
    array[i-1]=array[i];
  array[array.size()-1] = temp;
}

// #define _DEBUG

void regions( unsigned ndat, const float* data, 
	      unsigned istart, unsigned istop,
	      unsigned region_size, float cutoff,
	      std::vector<unsigned>& on_transitions,
	      std::vector<unsigned>& off_transitions )
{
  const int onpulse = 1;
  const int undefined = 0;
  const int off_pulse = -1;

  int current = undefined;
  int started = undefined;

  unsigned consecutive_on = 0;

  for (unsigned idat=istart; idat < istop+region_size; idat++) {

#ifdef _DEBUG
    cerr << "idat=" << idat << " ";
    if (current == onpulse)
      cerr << "on" << endl;
    if (current == off_pulse)
      cerr << "off" << endl;
    if (current == undefined)
      cerr << "?" << endl;
#endif

    if (data[idat%ndat] > cutoff)
      consecutive_on ++;
    else
      consecutive_on = 0;

    if (consecutive_on == region_size) {
      if (current == off_pulse) {
	on_transitions.push_back (idat+1-region_size);
#ifdef _DEBUG
	cerr << "TURNED ON" << endl;
#endif
      }
      current = onpulse;
    }

    if (consecutive_on == 0) {
      if (current == onpulse) {
	off_transitions.push_back (idat-1);
#ifdef _DEBUG
	cerr << "TURNED OFF" << endl;
#endif
      }
      current = off_pulse;
    }

    if (started == undefined && current != undefined)
      started = current;

  }

  if (on_transitions.size() != off_transitions.size())
    throw Error (InvalidState, "Pulsar::PeakConsecutive::build",
		 "transitions on=%u != off=%u",
		 on_transitions.size(),
		 off_transitions.size());

#ifdef _DEBUG
  cerr << "Pulsar::PeakConsecutive::build transitions="
       << on_transitions.size() << endl;
#endif

  if (off_transitions.size() == 0)
    return;

  // if the pulse was on in the first phase bin, then shift and correct
  if (started == onpulse)
    cyclic_shift (off_transitions);
}


void Pulsar::PeakConsecutive::build ()
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PeakConsecutive::build",
		 "Profile not set");

  Reference::To<PhaseWeight> baseline;

  if (baseline_estimator)
    baseline = baseline_estimator->baseline (profile);
  else
    baseline = profile->baseline();

  Estimate<double> mean = baseline->get_mean ();
  Estimate<double> var = baseline->get_variance ();
  Estimate<double> rms = sqrt(var);

  float cutoff = mean.get_value() + rms.get_value() * threshold;

  if (Profile::verbose)
    cerr << "Pulsar::PeakConsecutive::build baseline"
      " mean=" << mean << " rms=" << rms << " cut=" << cutoff << endl;

  unsigned nbin = profile->get_nbin();
  const float* amps = profile->get_amps();

  int start = 0;
  int stop = nbin;

  if (range_specified)
  {
    start = bin_start;
    stop  = bin_end;
    nbinify (start, stop, nbin);
  }

  on_transitions.resize (0);
  off_transitions.resize (0);
  
  regions (nbin, amps, start, stop, consecutive, cutoff,
	   on_transitions, off_transitions);

  // choose the grouping of peaks that minimizes their collective width

  unsigned min_width = nbin;
  unsigned offset = on_transitions.size() - 1;
  unsigned number = on_transitions.size();

  for (unsigned i=0; i < number; i++)
  {
    unsigned last = (i+offset) % number;

    unsigned on = on_transitions[i];
    unsigned off = off_transitions[last];
    if (off < on)
      off += nbin;

    unsigned width = off - on;

#ifdef _DEBUG
    cerr << "i=" << i << " last=" << last << " on=" << on
	 << " off=" << off << " width=" << width << endl;
#endif

    if (width < min_width)
    {
      min_width = width;
      bin_rise = on;
      bin_fall = off;
    }
  }
}


/*!
  \retval rise bin at which the cumulative power last remains above threshold
  \retval fall bin at which the cumulative power first falls below threshold
*/
void Pulsar::PeakConsecutive::get_indeces (int& rise, int& fall) const
{
  if (!built)
    const_cast<PeakConsecutive*>(this)->build ();

  rise = bin_rise;
  fall = bin_fall;
}

//! Retrieve the PhaseWeight
void Pulsar::PeakConsecutive::calculate (PhaseWeight* weight) 
{
  if (merge_regions)
  {
    RiseFall::calculate (weight);
    return;
  }

  assert( weight != 0 );

  build ();

  unsigned nbin = profile->get_nbin();
  weight->resize( nbin );
  weight->set_all( 0.0 );

  for (unsigned i=0; i < on_transitions.size(); i++)
  {
    int bin_rise = on_transitions[i];
    int bin_fall = off_transitions[i];

    // cerr << "rise=" << bin_rise << " fall=" << bin_fall << endl;

    nbinify (bin_rise, bin_fall, nbin);

    for (int ibin=bin_rise; ibin<bin_fall; ibin++)
      (*weight)[ibin % nbin] = 1.0;
  }
}

class Pulsar::PeakConsecutive::Interface 
  : public TextInterface::To<PeakConsecutive>
{
public:
  Interface (PeakConsecutive* instance)
  {
    if (instance)
      set_instance (instance);

    add( &PeakConsecutive::get_threshold,
	 &PeakConsecutive::set_threshold,
	 "threshold", "threshold above which consecutive points must fall" );

    add( &PeakConsecutive::get_consecutive,
	 &PeakConsecutive::set_consecutive,
	 "consecutive", "consecutive points required above threshold" );
  }

  std::string get_interface_name () const { return "consecutive"; }
};

//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::PeakConsecutive::get_interface ()
{
  // cerr << "Pulsar::PeakConsecutive::get_interface" << endl;
  return new Interface (this);
}
