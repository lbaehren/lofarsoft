/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <math.h>
#include "VanVleck.h"
#include "NormalDistribution.h"
#include "Error.h"

VanVleck::VanVleck()
{
  nlevel=0;
  pow_x=-1.0;
  pow_y=-1.0;
}

void VanVleck::set_nlevel(int n)
{
  if (n<2) 
    throw Error (InvalidParam, "VanVleck::set_nlevel",
        "Invalid nlevel=%d", n);
  levels.resize(n);
  thresh.resize(n-1);
  nlevel = n;
}

void VanVleck::set_uniform_threshold() 
{
  int idx;
  if (nlevel%2) {
    // Odd nlevel
    idx = nlevel/2;
    thresh[idx] = 0.5;
  } else {
    // Even nlevel
    idx = nlevel/2-1;
    thresh[idx] = 0.0;
  }
  for (int i=idx-1; i>=0; i--) { thresh[i] = thresh[i+1]-1.0; }
  for (unsigned i=idx+1; i<thresh.size(); i++) { thresh[i] = thresh[i-1]+1.0; }
}

void VanVleck::set_threshold(float *thr) 
{
}

void VanVleck::set_uniform_levels()
{
  int idx;
  if (nlevel%2) { 
    // Odd
    idx = nlevel/2;
    levels[idx] = 0.0;
  } else { 
    // Even
    idx = nlevel/2;
    levels[idx] = 0.5;
  }
  for (int i=idx-1; i>0; i--) { levels[i] = levels[i+1]-1.0; }
  for (int i=idx+1; i<nlevel; i++) { levels[i] = levels[i-1]+1.0; }
}

void VanVleck::set_canonical_levels()
{
  if (nlevel==2) { 
    levels[0] = -1.0;
    levels[1] = +1.0; // Probably not used much for pulsar data..
  } else if (nlevel==3) {
    levels[0] = -1.0; 
    levels[1] = 0.0; 
    levels[2] = 1.0; 
  } else if (nlevel==4) {
    levels[0] = -3.0; // Classic 2-bit output levels 
    levels[1] = -1.0;
    levels[2] = 1.0; 
    levels[3] = 3.0;
  } else {
    set_uniform_levels(); // All others get uniform levels
  }
}

void VanVleck::set_levels(float *lev) 
{
}

/* Theory for total power correction functions:
 */

double VanVleck::out_pow(double in_pow, double in_dc)
{
  double sum=0.0;
  NormalDistribution n;
  const double in_pow_1_2 = sqrt(in_pow);

  // Check for in_pow=0
  if (in_pow==0.0) { 
    if (in_dc==0.0) { 
      if (nlevel%2) return(levels[nlevel/2]*levels[nlevel/2]);
      else return(0.5*(levels[nlevel/2]*levels[nlevel/2] 
            + levels[nlevel/2-1]*levels[nlevel/2-1]));
    } else {
      unsigned ilev=0;
      for (ilev=0; ilev<thresh.size(); ilev++) {
        if (in_dc<thresh[ilev]) break;
      }
      return(levels[ilev]*levels[ilev]);
    }
  }

  // Pre-calculate cumulatve dist values at thresholds.
  std::vector<double> cdist(thresh.size());
  for (unsigned i=0; i<thresh.size(); i++) { 
      cdist[i] = n.cumulative_distribution((thresh[i]-in_dc)/in_pow_1_2);
  }

  // Sum over levels
  sum += levels[0] * levels[0] * cdist[0];
  for (int i=1; i<nlevel-1; i++) {
    sum += levels[i] * levels[i] * (cdist[i] - cdist[i-1]);
  }
  sum += levels[nlevel-1] * levels[nlevel-1] * (1.0 - cdist[nlevel-2]);

  return(sum);
}

double VanVleck::d_out_pow(double in_pow, double in_dc)
{
  double sum=0.0;
  NormalDistribution n;
  const double in_pow_1_2 = sqrt(in_pow);
  const double in_pow_3_2 = in_pow_1_2 * in_pow;

  // Check for in_pow=0
  if (in_pow==0.0) { return(0.0); }

  // Pre-calculate density values at thresholds.
  std::vector<double> dens(thresh.size());
  for (unsigned i=0; i<thresh.size(); i++) {
    dens[i] = n.density((thresh[i]-in_dc)/in_pow_1_2) * 
      (-0.5 * (thresh[i]-in_dc)/in_pow_3_2);
  }

  // Sum over levels
  sum += levels[0]*levels[0] * dens[0];
  for (int i=1; i<nlevel-1; i++) {
      sum += levels[i] * levels[i] * (dens[i] - dens[i-1]);
  }
  sum += levels[nlevel-1] * levels[nlevel-1] * (-dens[nlevel-2]);

  return(sum);
}

// Uses Newton's method and the previous two functions to 
// invert the power relationship.
double VanVleck::in_pow(double out_pow_meas)
{

  // If out of range, return negative.
  // Or should we throw a error here?
  const double pmax = levels.back()*levels.back();
  const double pmin = (nlevel%2)?0.0:levels.front()*levels.front();
  if (out_pow_meas<=pmin) { return(0.0); }
  if (out_pow_meas>=pmax) { return(-1.0); }

  // Newton parameters
  const int max_it = 100;
  const double frac_tol = 1e-10;

  // Initialize on some linear scale
  double result = ((out_pow_meas-pmin)/(pmax-pmin)+0.1) * thresh[0] * thresh[0];

  // Iterate
  int nit=0;
  double delta=0.0;
  while (nit<max_it) {
      delta = (out_pow_meas-out_pow(result))/d_out_pow(result);
      if (fabs(delta/result) < frac_tol) { return(result); }
      result += delta;
      nit++;
  }

  // Maxed out iterations
  return(-2.0);
}

/* Theory for correlation correction functions:
 */

void VanVleck::set_in_power(double px, double py)
{
  pow_x=px;
  pow_y=py;
  // compute coeffs here
  //compute_coeffs();
}

void VanVleck::correct_acf(std::vector<float> data)
{
}

void VanVleck::correct_ccf(std::vector<float> data)
{
}



