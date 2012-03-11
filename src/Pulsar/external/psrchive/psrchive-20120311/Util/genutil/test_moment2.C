/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/*
  This program tests that the expected value of the variance as a
  function of threshold, as predicted by NormalDistribution::moment2,
  matches the variance as a function of threshold measured in a simple
  simulation of normally distributed random data.
*/

#include "BoxMuller.h"
#include "NormalDistribution.h"

#include <vector>
#include <iostream>
#include <math.h>

using namespace std;

int main ()
{
  cerr << "Testing BoxMuller for basic shape of distribution" << endl;

  BoxMuller gasdev;

  unsigned nsamples = 1000000;
  unsigned nbins = 300;

  vector<double>   sumsq (nbins, 0);
  vector<unsigned> count (nbins, 0);

  for (unsigned i=0; i<nsamples; i++) {

    double x = gasdev();
    double xsq = x * x;

    for (unsigned ibin=0; ibin < nbins; ibin++) {

      double bound = 0.01 * (ibin + 1);
      double boundsq = bound * bound;

      if (xsq <= boundsq) {
	sumsq[ibin] += xsq;
	count[ibin] ++;
      }

    }

  }

  unsigned errors = 0;

  NormalDistribution normal;

  for (unsigned ibin=0; ibin < nbins; ibin++) {

    double bound = 0.01 * (ibin + 1);
    double exp = normal.moment2(-bound,bound);

    double var = sumsq[ibin]/count[ibin];
    double err = var * sqrt(2.0/count[ibin]);

    cout << bound << " " << exp << " " << var << " " << err
	 << " " << count[ibin] << endl;

    if ( fabs( var - exp ) > err * 2.0 ) {
      cerr << "unexpected result:\n"
	" threshold=" << bound << " variance=" << var << " +/- " << err <<
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

