/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/UnaryScalar.h"

using namespace std;

void MEAL::UnaryScalar::calculate (double& result,
				   std::vector<double>* grad)
{
  if (!model)
    throw Error (InvalidState, "MEAL::UnaryScalar::calculate",
		 "no model to evaluate");

  if (verbose)
    cerr << "MEAL::UnaryScalar::calculate" << endl;

  double x = model->evaluate (grad);

  result = function( x );

  if (verbose)
    cerr << "MEAL::UnaryScalar::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  double dydx = dfdx ( x );

  for (unsigned igrad=0; igrad<grad->size(); igrad++)
    (*grad)[igrad] *= dydx;

  if (verbose) {
    cerr << "MEAL::UnaryScalar::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}
