/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "JenetAnderson98A5.h"
#include <math.h>

// defined in JenetAnderson98.C
double ln_fact (unsigned n);

void JenetAnderson98::EquationA5::set_nsamp (unsigned L)
{
  if (losq.size() == L)
    return;

  losq.resize(L);
  hisq.resize(L);
  fact.resize(L);

  double lnLfact = ln_fact (L);

  JenetAnderson98 ja98;

  for (unsigned nlo=0; nlo<L; nlo++)
  {
    fact[nlo] = lnLfact - ln_fact(nlo) - ln_fact(L-nlo);

    ja98.set_Phi( double(nlo) / double(L) );
    losq[nlo] = ja98.get_lo() * ja98.get_lo();
    hisq[nlo] = ja98.get_hi() * ja98.get_hi();
  }
}


static double default_mean_Phi = 0.666656;

//! Return the digitized power, given <Phi>
double JenetAnderson98::EquationA5::evaluate (double mean_Phi)
{
  unsigned L = losq.size();

  if (!L)
    throw Error (InvalidState, "JenetAnderson98::EquationA5::evaluate",
		 "number of samples not set");

  double result = 0;
  dA5_dmean_Phi = 0;

  unsigned nlo_start = (unsigned) (default_mean_Phi * L);
  int increment = 1;

  for (unsigned dir=0; dir < 2; dir++) {

    for (unsigned nlo=nlo_start; nlo>1 && nlo<L; nlo += increment) {

      // compute equation A4
      double f = mean_Phi * losq[nlo] + (1-mean_Phi) * hisq[nlo];
      double df_dmean_Phi = losq[nlo] - hisq[nlo];

      double P = 0;
      double dP_dmean_Phi = 0;

      if (prob_Phi.size())
	P = prob_Phi[nlo];

      else
      {
	double Phi = double(nlo) / double(L);
	P = exp( fact[nlo] + L * ( log(pow (mean_Phi, Phi)) +
				   log(pow (1.0-mean_Phi, 1.0-Phi)) ) );

	dP_dmean_Phi = P * ( nlo/mean_Phi - (L-nlo)/(1.0-mean_Phi) );
      }

      double term = f * P;

      result += term;
      dA5_dmean_Phi += dP_dmean_Phi * f + P * df_dmean_Phi;

      if (term*term < 1e-30)
	break;

    }

    nlo_start --;
    increment = -1;

  }

  return result;
}

/*! Inverts equation A5 using the Newton-Raphson method */
double JenetAnderson98::EquationA5::invert (double sigma_hat)
{
  double guess = default_mean_Phi;

  for (unsigned i=0; i<50; i++)
  {
    double dx = (evaluate(guess) - sigma_hat) / dA5_dmean_Phi;
    guess -= dx;
    if (fabs (dx) <= fabs(guess)*1e-10)
      return guess;
  }

  throw Error (InvalidState, "JenetAnderson98::EquationA5::invert",
	       "maximum iterations exceeded; sigma_hat=%lf", sigma_hat);
}


