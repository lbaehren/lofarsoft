/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ExponentialDistribution.h"
#include <math.h>

//! Get the value of the probability density at x
double ExponentialDistribution::density (double x)
{
  if (x < 0)
    return 0;
  else
    return exp (-x);
}

//! Get the value of the distribution function at x
/*! The distribution, \f$ D(x) = \int_{-\inf}^x P(t) dt \f$ */
double ExponentialDistribution::cumulative_distribution (double x)
{
  if (x < 0)
    return 0;
  else
    return 1.0 - exp(-x);
}

//! Get the first moment of the distribution averaged from 0 to x
/*! i.e. \f$ \int_{-inf}^x t P(t) dt \over D(x) \f$ */
double ExponentialDistribution::cumulative_mean (double x)
{
  return 1.0 - x * density (x) / cumulative_distribution(x);
}

//! Get the second moment of the distribution averaged from 0 to x
/*! i.e. \f$ \int_{-inf}^x t^2 P(t) dt \over D(x) \f$ */
double ExponentialDistribution::cumulative_moment2 (double x)
{
  return 2.0 - x * (2+x) * density(x) / cumulative_distribution(x);
}

