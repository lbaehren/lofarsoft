/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/BinaryScalar.h"
#include "MEAL/ProjectGradient.h"

using namespace std;

void MEAL::BinaryScalar::calculate (double& result,
					   std::vector<double>* grad)
{
  if (!arg1 || !arg2)
    throw Error (InvalidState, "MEAL::BinaryScalar::calculate",
		 "both arguments are not set");

  if (verbose)
    cerr << "MEAL::BinaryScalar::calculate" << endl;

  std::vector<double> grad1;
  std::vector<double> grad2;

  std::vector<double>* grad1_ptr = 0;
  std::vector<double>* grad2_ptr = 0;

  if (grad)  {
    grad1_ptr = &grad1;
    grad2_ptr = &grad2;
  }

  double x1 = arg1->evaluate (grad1_ptr);
  double x2 = arg2->evaluate (grad2_ptr);

  result = function( x1, x2 );

  if (verbose)
    cerr << "MEAL::BinaryScalar::calculate result\n"
      "   " << result << endl;

  if (!grad)
    return;

  double dydx1 = partial_arg1 ( x1, x2 );
  double dydx2 = partial_arg2 ( x1, x2 );

  unsigned igrad;
  for (igrad=0; igrad<grad1.size(); igrad++)
    grad1[igrad] *= dydx1;

  for (igrad=0; igrad<grad2.size(); igrad++)
    grad2[igrad] *= dydx2;

  unsigned ngrad = get_nparam();
  grad->resize (ngrad);

  for (igrad=0; igrad<ngrad; igrad++)
    (*grad)[igrad] = 0.0;

  // map the model gradient
  ProjectGradient (arg1, grad1, *(grad));
  ProjectGradient (arg2, grad2, *(grad));

  if (verbose) {
    cerr << "MEAL::BinaryScalar::calculate gradient\n";
    for (unsigned i=0; i<grad->size(); i++)
      cerr << "   " << i << ":" << get_infit(i) << "=" << (*grad)[i] << endl;
  }

}
