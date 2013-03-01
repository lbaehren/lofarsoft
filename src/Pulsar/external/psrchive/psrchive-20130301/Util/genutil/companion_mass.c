/***************************************************************************
 *
 *   Copyright (C) 2001 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <math.h>
#include <stdio.h>

/* ************************************************************************
   companion_mass
      solves mass function for m2, using the Newton-Raphson method

   where:  mf = mass function
           m1 = primary mass
	   si = sin(i) (i = inclination angle)

   solves: (m1+m2)^2 = (m2*si)^3 / mf

   returns -1 on error
   ************************************************************************ */

#define sqr(x) ((x)*(x))
#define cube(x) ((x)*(x)*(x))

double companion_mass (double mf, double sini, double m1)
{
  double guess = m1;
  double dx = 0.0;
  double eq = 0.0;
  double deq_dm2 = 0.0;
  int gi = 0;

  for (gi=0; gi<10000; gi++) {
    eq = sqr(m1+guess) - cube(guess*sini) / mf;
    deq_dm2 = 2.0*(m1+guess) - 3.0 * sqr(guess*sini) / mf;

    dx = eq / deq_dm2;
    guess -= dx;

    if (fabs (dx) <= fabs(guess)*1e-10)
      return guess;
  }
  fprintf (stderr, "companion_mass: maximum iterations exceeded - %lf\n",
	   fabs (dx/guess));
  return -1.0;
}

