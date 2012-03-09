/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Estimate.h"
#include "tostring.h"
#include <math.h>

using namespace std;

string latex (const Estimate<double>& estimate)
{
  double val = estimate.get_value();
  double err = estimate.get_error();

  string value;

  string error;

  if (err > 0.0)
  {
    int digits = (int) -floor(log(err)/log(10.0));
    // cerr << "digits=" << digits << endl;

    double scale = pow (10.0, digits);
    err = rint (err*scale);
    if (err == 10)
      err = 1;

    error = "(" + tostring (err) + ")";

    val = rint(val*scale)/scale;

    value = tostring (val, digits, std::ios_base::fixed);
  }
  else
     value = tostring (val);

  string::size_type exp = value.find('e');
  string exponent;

  if (exp != string::npos)
  {
    value = value.substr(0,exp);
    exponent = "$\\times 10^{" + value.substr(exp+1) + "}$";
  }

  return value + error + exponent;
}
