/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Invariant.h"

using namespace std;

int main ()
{
  MEAL::Invariant invariant;

  Estimate<double> I (0.967525,0.000659444);
  Estimate<double> Q (0.202817,0.000658172);
  Estimate<double> U (0.0604878,0.000660645);
  Estimate<double> V (-0.0498089,0.00068193);

  Stokes< Estimate<double> > stokes (I,Q,U,V);

  invariant.set_Stokes( stokes );
 
  cerr << "simple invariant = " << stokes.invariant() << endl
       << "correct invariant = " << invariant.get_correct_result() << endl;

  /*
    the bias due to noise (bias ~= -3 * variance) has been removed
    from correct invariant; the correct invariant also better
    calculates the variance of the result (that is, Estimate<T> does
    not differentiate between x*y and x*x; the former requires adding
    the relative error in quadrature, the latter does not).
  */

  return 0;
}

