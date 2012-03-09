/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
using namespace std;
#include <math.h>

#include "Pulsar/Profile.h"

// defined in Profile.C
void nbinify (int& istart, int& iend, int nbin);

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::stats
//
/*! Returns the mean, variance, and variance of the mean over the specified
  interval.
  \retval mean the mean of the interval
  \retval variance the variance of the interval
  \retval varmean the variance of the mean of the interval
  \param istart the first bin of the interval
  \param iend one greater than the last bin of the interval
*/
void Pulsar::Profile::stats (double* mean, double* variance, double* varmean,
			     int istart, int iend) const
{
  if (verbose) cerr << "Pulsar::Profile::stats"
		 " istart=" << istart << " iend=" << iend << endl;
  
  unsigned counts = 0; 
  double tot = 0;
  double totsq = 0;

  unsigned nbin = get_nbin();
  const float* amps = get_amps();

  nbinify (istart, iend, nbin);

  if (verbose) cerr << "Pulsar::Profile::stats"
		 " start:" << istart <<
		 " stop:" << iend << endl;
  
  for (int ibin=istart; ibin < iend; ibin++) {
    double value = amps[ibin%nbin];
    tot += value;
    totsq += value*value;
    counts ++;
  }

  if (!counts)
    throw Error (InvalidRange, "Pulsar::Profile::stats",
		 "no samples in %d -> %d", istart, iend);

  //
  // variance(x) = <(x-<x>)^2> * N/(N-1) = (<x^2>-<x>^2) * N/(N-1)
  //
  double mean_x   = tot / double(counts);
  double mean_xsq = totsq / double(counts);


  double var_x = 0.0;

  if (counts > 1)
    var_x = (mean_xsq - mean_x*mean_x) * double(counts)/double(counts-1);

  double var_mean = var_x / double(counts);

  if (mean)
    *mean = mean_x;
  if (variance)
    *variance = var_x;
  if (varmean)
    *varmean = var_mean;

  if (verbose)
    cerr << "Pulsar::Profile::stats return mean=" << mean_x 
	 << " var=" << var_x << " varmean=" << var_mean << endl;
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::stats
//
/*! 
  \param phase centre of region
  \retval mean the mean of the interval
  \retval variance the variance of the interval
  \retval varmean the variance of the mean of the interval
  \param phase centre of region over which statistics will be calculated
  \param duty_cycle width of region
  */
void Pulsar::Profile::stats (float phase,
			     double* mean, 
			     double* variance,
			     double* varmean,
			     float duty_cycle) const
{
  if (verbose) cerr << "Pulsar::Profile::stats phase=" << phase
		    << " duty_cycle=" << duty_cycle << endl;

  unsigned nbin = get_nbin();

  int start_bin = int ((phase - 0.5 * duty_cycle) * nbin);
  int stop_bin = int ((phase + 0.5 * duty_cycle) * nbin);

  stats (mean, variance, varmean, start_bin, stop_bin);
}

/////////////////////////////////////////////////////////////////////////////
//
// Pulsar::Profile::mean
//
/*! 
  \param phase centre of region
  \param duty_cycle width of region
  \return mean of region
*/
double Pulsar::Profile::mean (float phase, float duty_cycle) const
{
  double result; 
  stats (phase, &result, 0, 0, duty_cycle);
  return result;
} 

