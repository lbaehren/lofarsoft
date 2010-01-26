/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This program tests that the expected value of the mean as a function
  of threshold, as predicted by the cumulative_mean method of the
  ExponentialDistribution class, matches the mean as a function of
  threshold measured in a simple simulation of exponentially
  distributed random data.
*/

#include "BoxMuller.h"
#include "ExponentialDistribution.h"

#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

int main ()
{
  cerr << "Testing ExponentialDistribution class" << endl;

  BoxMuller gasdev;

  ExponentialDistribution exponential;

  unsigned nsamples = 500000;
  unsigned nbins = 300;

  vector<double>   total (nbins, 0);
  vector<unsigned> count (nbins, 0);

  for (unsigned i=0; i<nsamples; i++) {

    // an exponential distribution is chi-squared with 2 d.o.f

    double x = gasdev();
    double xsq = x * x;

    x = gasdev();
    xsq += x * x;

    // mean value of 1.0 required

    xsq /= 2.0;

    for (unsigned ibin=0; ibin < nbins; ibin++) {

      double bound = 0.01 * (ibin + 1);

      if (xsq <= bound) {
	total[ibin] += xsq;
	count[ibin] ++;
      }

    }

  }

  unsigned errors = 0;

  for (unsigned ibin=0; ibin < nbins; ibin++) {

    double bound = 0.01 * (ibin + 1);
    double exp = exponential.cumulative_mean (bound);

    double mean = total[ibin]/count[ibin];
    double err = mean * sqrt(2.0/count[ibin]);

    cout << bound << " " << exp << " " << mean << " " << err
	 << " " << count[ibin] << endl;

    if ( fabs( mean - exp ) > err * 2.0 ) {
      cerr << "unexpected result:\n"
	" threshold=" << bound << " mean=" << mean << " +/- " << err <<
	" expected=" << exp << endl;
      errors ++;
    }

  }

  double percentage_error = double(errors) / double(nbins) * 100.0;
  cerr << "Percentage error = " << percentage_error << " %" << endl;

  if (percentage_error > 5.0) {
    cerr << "Unacceptable error" << endl;
    return -1;
  }

  return 0;
}

