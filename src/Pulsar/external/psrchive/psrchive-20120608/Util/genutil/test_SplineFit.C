/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;

#include "SplineFit.h"
#include "BoxMuller.h"

double func(double x) {
  return 0.00834*x + 5.6678 + sin(2.0*M_PI*x/76.0);
}

int main () 
{

  BoxMuller noise;
  SplineFit sf;

  // Make fake data set
  int npts = 1024;
  double nfac = 0.25;
  std::vector<double> x;
  std::vector< Estimate<double> > y;
  x.resize(npts);
  y.resize(npts);
  for (unsigned i=0; i<npts; i++) {
    x[i] = (double)i;
    double ytmp = func(x[i]);
    y[i].set_value(ytmp + nfac*noise());
    y[i].set_variance(nfac*nfac);
    // Skip some points
    if (i>540 && i<600) continue;
    sf.add_data(x[i], y[i]);
  }

  sf.set_uniform_breaks(npts/16);
  sf.compute();

  for (unsigned i=0; i<npts; i++) {
    printf("%+.6e  %+.6e %+.6e  %+.6e\n", x[i], 
        y[i].get_value(), y[i].get_error(), 
        sf.evaluate(x[i]));
  }

  return 0;
}
