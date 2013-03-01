/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/LeastSquares.h"

MEAL::LeastSquares::LeastSquares ()
{
  // should be ok
  maximum_iterations = 50;

  // do not converge based on chisq
  convergence_chisq = 0.0;

  // do not cull solutions
  maximum_reduced = 0.0;

  // converge when relative change in parameters is less than 1%
  convergence_delta = 0.01;

  // set during solve
  iterations = 0;
  best_chisq = 0.0;
  nfree = 0;

  // used during solve
  debug = false;
  solved = false;
  singular = false;
}

void MEAL::LeastSquares::set_convergence_chisq (float chisq)
{
  convergence_chisq = chisq;
}

void MEAL::LeastSquares::set_convergence_delta (float delta)
{
  convergence_delta = delta;
}

void MEAL::LeastSquares::set_maximum_reduced_chisq (float max)
{
  maximum_reduced = max;
}

