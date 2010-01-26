/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Error.h"
#include "Reference.h"

#include "Pulsar/TwoBitStats.h"
#include "JenetAnderson98.h"
#include "templates.h"

using namespace std;

//! Default constructor
Pulsar::TwoBitStats::TwoBitStats ()
  : Extension ("TwoBitStats")
{
  nsample = 0;
  ndig = 0;

  threshold = JenetAnderson98::get_optimal_spacing(2);
  cutoff_sigma = 0.0;
}

//! Copy constructor
Pulsar::TwoBitStats::TwoBitStats (const TwoBitStats& stats)
  : Extension ("TwoBitStats")
{
  operator=(stats);
}

//! Operator =
const Pulsar::TwoBitStats&
Pulsar::TwoBitStats::operator= (const TwoBitStats& stats)
{
  nsample = stats.nsample;
  ndig    = stats.ndig;

  threshold    = stats.threshold;
  cutoff_sigma = stats.cutoff_sigma;

  histogram = stats.histogram;

  return *this;
}

//! Get the number of time samples used to estimate undigitized power
unsigned Pulsar::TwoBitStats::get_nsample () const
{
  return nsample;
}

//! Get the number of digitizers
unsigned Pulsar::TwoBitStats::get_ndig () const
{
  return ndig;
}

//! Set the sampling threshold as a fraction of the noise power
void Pulsar::TwoBitStats::set_threshold (float _threshold)
{
  threshold = _threshold;
}
    
//! Set the cut off power used for impulsive interference excision
void Pulsar::TwoBitStats::set_cutoff_sigma (float _cutoff_sigma)
{
  cutoff_sigma = _cutoff_sigma;
}

//! Set the number of samples per histogram point and number of digitizers
void Pulsar::TwoBitStats::resize (unsigned _nsample, unsigned _ndig)
{
  // ensure that none of the above are zero
  if (! (_nsample && _ndig))
    throw Error (InvalidParam, "Pulsar::TwoBitStats::resize",
		 "nsample=%d ndig=%d", _nsample, _ndig);

  nsample = _nsample;
  ndig = _ndig;

  histogram.resize (ndig);
  for (unsigned idig=0; idig<histogram.size(); idig++)
    histogram[idig].resize (nsample);
}

void Pulsar::TwoBitStats::zero()
{
  for (unsigned idig=0; idig<histogram.size(); idig++)
    for (unsigned ichan=0; ichan<nsample; ichan++)
      histogram[idig][ichan] = 0.0;
}

//! Get the specified histogram
const vector<float>&
Pulsar::TwoBitStats::get_histogram (unsigned idig) const
{
  range_check (idig, "Pulsar::TwoBitStats::get_histogram");
  return histogram[idig];
}

//! Set the specified histogram
void Pulsar::TwoBitStats::set_histogram (const vector<float>& data,
					 unsigned idig)
{
  range_check (idig, "Pulsar::TwoBitStats::set_histogram");

  if (data.size() != nsample)
    throw Error (InvalidParam, "Pulsar::TwoBitStats::set_histogram",
		 "data.size=%d >= nsample=%d", data.size(), nsample);

  histogram[idig] = data;
}

void Pulsar::TwoBitStats::range_check (unsigned idig, const char* method) const
{
  if (idig >= ndig)
    throw Error (InvalidRange, method, "idig=%d >= ndig=%d", idig, ndig);
}

double Pulsar::TwoBitStats::get_mean_Phi (unsigned idig) const
{
  range_check (idig, "Pulsar::TwoBitStats::get_mean_Phi");
  return histomean (histogram[idig]);
}

double Pulsar::TwoBitStats::get_distortion (unsigned idig) const
{
  vector<float> theory;

  JenetAnderson98 ja98;

  ja98.set_mean_Phi( get_mean_Phi(idig) );
  ja98.get_prob_Phi ( get_nsample(), theory );

  double nweights = sum (histogram[idig]);
  
  double distortion = 0.0;

  for (unsigned isamp=0; isamp < nsample; isamp++) {
    double normval = histogram[idig][isamp] / nweights;
    double offmodel = normval - theory[isamp];
    distortion += offmodel * offmodel;
  }

  return distortion;
}
