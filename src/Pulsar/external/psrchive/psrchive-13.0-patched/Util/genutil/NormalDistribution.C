/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "NormalDistribution.h"
#include <math.h>

static const double norm = 1.0 / sqrt (2.0 * M_PI);

//! Get the value of the probability density at x
double NormalDistribution::density (double x)
{
  return norm * exp (-0.5*(x*x));
}

//! Get the value of the distribution function at x
double NormalDistribution::cumulative_distribution (double x)
{
  return 0.5 * (1.0 + erf (x * M_SQRT1_2));
}

//! Get the first moment of the distribution averaged from 0 to x
double NormalDistribution::cumulative_mean (double x)
{
  return - density (x) / cumulative_distribution(x);
}

//! Get the second moment of the distribution averaged from 0 to x
double NormalDistribution::cumulative_moment2 (double x)
{
  return 1.0 - x * density(x) / cumulative_distribution(x);
}

/*! "Raw" cumulative moments defined as
 *
 *   \f$ C_m(x) = \int_0^x z^m P(z) dz \f$
 *
 * can be computed for the normal distribution to be:
 *
 *   \f$ C_m(x) = \sqrt{2^{m-2}/\pi} * \gamma(1+(m-1)/2, x^2/2) \f$
 *
 * where \f$\gamma(a,x)\f$ is the (lower) incomplete gamma function.  For odd
 * m, this becomes really easy since a is then an integer.  This is used in the
 * following functions.
 *
 * I called these raw rather than unnormalized since they are computed using a
 * normalized PDF (sigma=1.0), but are not divided by the corresponding
 * cumulative distribution.
 */

//! Get the raw first moment of the distribution averaged from 0 to x.
double NormalDistribution::cumulative_mean_raw(double x)
{
  double u = 0.5*x*x;
  double igamma1 = 1.0 - exp(-u); // Inc. gamma fn., a=1
  return (1.0/sqrt(2.0*M_PI)) * igamma1;
}

//! Get the raw third moment of the distribution averaged from 0 to x.
double NormalDistribution::cumulative_moment3_raw(double x)
{
  double u = 0.5*x*x;
  double igamma2 = 1.0 - (1.0 + u)*exp(-u); // Inc. gamma fn., a=2
  return sqrt(2.0/M_PI) * igamma2;
}

