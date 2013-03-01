/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Probability.h"

//! Get the first moment of the distribution averaged from x1 to x2
double Probability::mean (double x1, double x2)
{
  double d1 = cumulative_distribution(x1);
  double d2 = cumulative_distribution(x2);

  return (d2 * cumulative_mean(x2) - d1 * cumulative_mean(x1)) / (d2 - d1);
}

//! Get the second moment of the distribution averaged from x1 to x2
double Probability::moment2 (double x1, double x2)
{
  double d1 = cumulative_distribution(x1);
  double d2 = cumulative_distribution(x2);

  return (d2*cumulative_moment2(x2) - d1*cumulative_moment2(x1)) / (d2 - d1);
}
