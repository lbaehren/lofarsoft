/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <math.h>
#include <stdio.h>

#include "orbital.h"

/* ************************************************************************
   eccentric_anomaly
      solves Kepler's equation using the Newton-Raphson method

   where:  M = mean_anomaly
           E = eccentric_anomaly
           e = eccentricity

   solves: M = E - e sin(E)     [ using:  dM/dE = 1 - e cos(E) ]
   ************************************************************************ */
double eccentric_anomaly (double mean_anomaly, double eccentricity)
{
  double guess = mean_anomaly;
  double dx = 0;
  int gi = 0;

  for (; gi<10000; gi++) {
    dx = (guess - eccentricity * sin(guess) - mean_anomaly)
      / (1 - eccentricity * cos(guess));
    guess -= dx;
    if (fabs (dx) <= fabs(guess)*1e-10)
      return guess;
  }
  fprintf (stderr, "eccentric_anomaly: maximum iterations exceeded - %lf\n",
	   fabs (dx/guess));
  return guess;
}

