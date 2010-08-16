/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeight.h"
#include "Pulsar/Profile.h"

#include <float.h>

#include <algorithm>

using namespace std;

void Pulsar::PhaseWeight::init ()
{
  reference_frequency = 0.0;
  built = false;
  median_computed = false;
  median = 0;
  median_diff_computed = false;
  median_diff = 0;
}

Pulsar::PhaseWeight::PhaseWeight ()
{
  init ();
}

Pulsar::PhaseWeight::PhaseWeight (const Profile* profile)
{
  init ();

  set_Profile (profile);
  weight.resize( profile->get_nbin() );
  set_all (1.0);
}

Pulsar::PhaseWeight::PhaseWeight (unsigned nbin)
  : weight (nbin)
{
  init ();
}

Pulsar::PhaseWeight::PhaseWeight (unsigned nbin, float set_all)
  : weight (nbin, set_all)
{
  init ();
}

Pulsar::PhaseWeight::PhaseWeight (const PhaseWeight& pm)
{
  init ();
  operator = (pm);
}

Pulsar::PhaseWeight::PhaseWeight (const vector<float>& _weight)
{
  init ();
  weight = _weight;
}

Pulsar::PhaseWeight::~PhaseWeight ()
{
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator = (const PhaseWeight& pm)
{
  weight = pm.weight;
  reference_frequency = pm.reference_frequency;
  built = false;
  return *this;
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator += (const PhaseWeight& pm)
{
  if (weight.size() != pm.weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::operator +=",
		 "weight size=%d != other weight size=%d",
		 weight.size(), pm.weight.size());

  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight[ipt] += pm.weight[ipt];

  built = false;
  return *this;
}

const Pulsar::PhaseWeight& 
Pulsar::PhaseWeight::operator *= (const PhaseWeight& pm)
{
  if (weight.size() != pm.weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::operator *=",
		 "weight size=%d != other weight size=%d",
		 weight.size(), pm.weight.size());

  // cerr << "Pulsar::PhaseWeight::operator *=" << endl;

  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight[ipt] *= pm.weight[ipt];

  built = false;
  return *this;
}

void Pulsar::PhaseWeight::set_all (float value)
{
  for (unsigned ipt=0; ipt<weight.size(); ipt++)
    weight[ipt] = value;
  built = false;
}
 
//! Retrieve the weights
void Pulsar::PhaseWeight::get_weights (vector<float>& weights) const
{
  weights = weight;
}

double Pulsar::PhaseWeight::get_weight_sum () const
{
  double total = 0.0;
  for (unsigned i=0; i<weight.size(); i++)
    total += weight[i];
  return total;
}

double Pulsar::PhaseWeight::get_weight_max () const
{
  double max = 0.0;
  for (unsigned i=0; i<weight.size(); i++)
    if (weight[i] > max)
      max = weight[i];
  return max;
}

float Pulsar::PhaseWeight::get_max () const
{
  check_Profile ("get_max");

  unsigned nbin = profile->get_nbin();

  check_weight (nbin, "get_max");

  const float* amps = profile->get_amps();

  float max = -FLT_MAX;
  for (unsigned i=0; i<nbin; i++)
    if (weight[i] && amps[i] > max)
      max = amps[i];

  return max;
}

float Pulsar::PhaseWeight::get_min () const
{
  check_Profile ("get_min");

  unsigned nbin = profile->get_nbin();

  check_weight (nbin, "get_min");

  const float* amps = profile->get_amps();

  float min = FLT_MAX;
  for (unsigned i=0; i<nbin; i++)
    if (weight[i] && amps[i] < min)
      min = amps[i];

  return min;
}

//! fill the non_zero vector
void Pulsar::PhaseWeight::fill_non_zero (const char* method) const
{
  check_Profile (method);
  unsigned nbin = profile->get_nbin();
  check_weight (nbin, method);

  const float* amps = profile->get_amps();

  non_zero.resize( 0 );
  for (unsigned i=0; i<nbin; i++)
    if (weight[i])
      non_zero.push_back( amps[i] );
}

//! Get the median amplitude with non-zero weight
float Pulsar::PhaseWeight::get_median () const
{
  if (median_computed)
    return median;

  fill_non_zero ("get_median");

  unsigned size = non_zero.size();

  if (!size)
    median = 0;
  else {
    unsigned mid = size/2;
    std::nth_element( non_zero.begin(), non_zero.begin()+mid, non_zero.end() );
    median = non_zero[ mid ];
  }

  median_computed = true;
  return median;
}

//! Get the median difference between the median and non-zero amplitudes
float Pulsar::PhaseWeight::get_median_difference () const
{
  get_median ();

  unsigned size = non_zero.size();

  for (unsigned i=0; i<size; i++)
    non_zero[i] = fabs( non_zero[i] - median );

  if (!size)
    median_diff = 0;
  else {
    unsigned mid = size/2;
    std::nth_element( non_zero.begin(), non_zero.begin()+mid, non_zero.end() );
    median_diff = non_zero[ mid ];
  }

  median_diff_computed = true;
  return median_diff;
}

//! Set the Profile to which the weights apply
void Pulsar::PhaseWeight::set_Profile (const Profile* _profile)
{
  profile = _profile;
  built = false;
  median_computed = false;
  median_diff_computed = false;
}

//! Get the weighted total of the Profile amplitudes
double Pulsar::PhaseWeight::get_weighted_sum () const
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PhaseWeight::get_weighted_sum",
		 "Profile not set");

  unsigned nbin = profile->get_nbin();

  if (nbin != weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::stats",
		 "weight size=%d != profile nbin=%d",
		 weight.size(), nbin);

  const float* amps = profile->get_amps();

  double total = 0.0;
  for (unsigned ibin=0; ibin<weight.size(); ibin++)
    total += weight[ibin] * amps[ibin];

  return total;
}

//! Get the weighted mean of the Profile
Estimate<double> Pulsar::PhaseWeight::get_mean () const
{
  if (!built)
    const_cast<PhaseWeight*>(this)->build();
  return mean;
}

float Pulsar::PhaseWeight::get_avg () const
{
  return get_mean().get_value();
}

//! Get the weighted variance of the Profile
Estimate<double> Pulsar::PhaseWeight::get_variance () const
{
  if (!built)
    const_cast<PhaseWeight*>(this)->build();
  return variance;
}

float Pulsar::PhaseWeight::get_rms () const
{
  return sqrt( get_variance().get_value() );
}

void
Pulsar::PhaseWeight::check_Profile (const char* method) const
{
  if (!profile)
    throw Error (InvalidState, "Pulsar::PhaseWeight::" + string(method),
		 "Profile not set");
}

void
Pulsar::PhaseWeight::check_weight (unsigned nbin, const char* method) const
{
  if (nbin != weight.size())
    throw Error (InvalidState, "Pulsar::PhaseWeight::" + string(method),
		 "weight size=%d != profile nbin=%d",
		 weight.size(), nbin);
}

void Pulsar::PhaseWeight::build ()
{
  check_Profile ("build");

  stats (profile, &(mean.val), &(variance.val), &(mean.var), &(variance.var));

  if (Profile::verbose)
    cerr << "Pulsar::PhaseWeight::build mean=" << mean << " var=" << variance
	 << endl;

  built = true;
}

//! Weigh the Profile amplitudes by the weights
void Pulsar::PhaseWeight::weight_Profile (Profile* data) const
{
  unsigned nbin = data->get_nbin();

  check_weight (nbin, "weight_Profile");

  float* amps = data->get_amps();

  for (unsigned ibin=0; ibin < nbin; ibin++)
    amps[ibin] *= weight[ibin];
}

void Pulsar::PhaseWeight::stats (const Profile* profile,
				 double* mean, double* var,
				 double* varmean, double* varvar) const
{
  if (Profile::verbose)
    cerr << "Pulsar::PhaseWeight::stats" << endl;
  
  unsigned nbin = profile->get_nbin();
  unsigned ibin = 0;

  check_weight (nbin, "stats");

  const float* amps = profile->get_amps();

  double totwt = 0; 
  double mu   = 0;

  unsigned count = 0;

  for (ibin=0; ibin < nbin; ibin++) {

    double value = double(weight[ibin]) * double(amps[ibin]);

    mu += value;
    totwt += weight[ibin];

    if (weight[ibin])
      count ++;
  }

  if (totwt == 0) {

    if (Profile::verbose)
      cerr << "Pulsar::PhaseWeight::stats total weight == 0" << endl;
 
    if (mean)
      *mean = 0;
    if (var)
      *var = 0;
    if (varmean)
      *varmean = 0;
    if (varvar)
      *varvar = 0;
    return;
  }

  // cerr << "weight=" << totwt << " sum=" << get_weight_sum() << endl;

  mu /= totwt;

  double mu2  = 0;
  double mu4  = 0;

  for (ibin=0; ibin < nbin; ibin++) {
    double value = double(weight[ibin]) * double(amps[ibin] - mu);
    double value2 = value * value;
    mu2  += value2;
    mu4  += value2*value2;
  }

  mu2 /= totwt;
  mu4 /= totwt;

  // bias-corrected sample variance
  double correction = totwt;
  if (count > 1)
    correction = double(count) / double(count-1);

  double var_x = mu2 * correction;

  double totwt3 = totwt * totwt * totwt;

  double totwt_1 = totwt / correction;

  double var_var = (totwt_1*totwt_1*mu4 - totwt_1*(totwt-3)*mu2*mu2)/totwt3;

  if (mean)
    *mean = mu;
  if (var)
    *var  = var_x;

  if (varmean)
    *varmean = var_x / totwt;
  if (varvar)
    *varvar = var_var;

  if (Profile::verbose) cerr << "Pulsar::PhaseWeight::stats return" << endl;
}
