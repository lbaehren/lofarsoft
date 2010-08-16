/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "templates.h"
#include <iostream>
#include <math.h>

using namespace std;

int main ()
{
  vector<float> x (5);

  x[0] = 1.2;
  x[1] = 3.4;
  x[2] = 5.6;
  x[3] = 7.8;
  x[4] = 9.0;

  double dsum = sum(x.begin(),x.end(),dsum);
  float fsum = sum(x);

  double esum = 27.0;

  if (fabs(fsum - dsum) > 1e-6 || fabs(dsum - esum) > 1e-6) {
    cerr << "sum template error" << endl;
    cerr << "fsum=" << fsum << " dsum=" << dsum << " expect=" << esum
         << " diff=" << dsum-esum << endl;
    return -1;
  }

  double dvar = variance(x.begin(),x.end(),dvar);
  float fvar = variance(x);

  if (fabs(fvar - dvar) > 1e-5) {
    cerr << "var template error" << endl;
    cerr << "fvar=" << fvar << " dvar=" << dvar
         << " diff=" << dvar-fvar << endl;
    return -1;
  }

  double evar = 10.1;

  if (fabs(dvar - evar) > 1e-6) {
    cerr << "var template error" << endl;
    cerr << " dvar=" << dvar << " expect=" << evar
         << " diff=" << dvar-evar << endl;
    return -1;
  }

  cerr << "sum and variance templates pass all tests" << endl;

  return 0;
}
