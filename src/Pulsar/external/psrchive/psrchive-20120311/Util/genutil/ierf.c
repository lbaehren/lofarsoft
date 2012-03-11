/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <math.h>
#include <stdio.h>
#include <signal.h>

#include "ierf.h"

/* ************************************************************************
   ierf -
   returns the inverse-error function using the Newton-Raphson method.
   erf(x) is monotonically increasing on -1 -> 1, and mostly linear
   around zero, so this should converge like magic (in tests: 9 iterations).

   As well, the first derivative of erf(x) is simply exp(-x^2)
   ************************************************************************ */

double ierf (double erfx)
{
  /* use the linear behaviour around erf(0) to get a good first guess */
  double guess = erfx;
  double dx = 0;
  int gi = 0;
  int gmax = 50;

  if (erfx > 1.0 || erfx < -1.0)
    raise (SIGFPE);

  for (gi=0; gi<gmax; gi++) {
    dx = (erf(guess) - erfx) / exp (-guess*guess);
    guess -= dx;
    if (fabs (dx) <= fabs(guess)*1e-10)
      return guess;
  }
  fprintf (stderr, "ierf: maximum iterations exceeded - %lf error\n", dx);
  return guess;
}

