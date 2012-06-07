/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/BasisRule.h"
#include "MEAL/Polar.h"
#include "MEAL/Polar.h"

using namespace std;
using namespace MEAL;

int main (int argc, char** argv) try {

  //Function::very_verbose = true;
  Function::verbose = true;

  cerr << "Instantiating BasisRule<7,Complex2>" << endl;
  BasisRule<7, Complex2> basis;

  cerr << "Instantiating Polar" << endl;
  Polar polar;

  cerr << "Calling BasisRule<7,Complex2>::set_model" << endl;
  basis.set_model (&polar);

  if (basis.get_nparam() != 7 + polar.get_nparam()) {
    cerr << "BasisRule<7,Complex2>::get_nparam = " << basis.get_nparam()
	 << " != " << 7 + polar.get_nparam() << endl;
    return -1;
  }

  basis.set_param (0, 1.0);

  basis.set_param (1, 0.1);
  basis.set_param (2, 0.2);
  basis.set_param (3, 0.3);

  basis.set_param (4, 0.1);
  basis.set_param (5, 0.2);
  basis.set_param (6, 0.3);

  vector< Jones<double> > grad;
  Jones<double> result = basis.evaluate (&grad);

  if (result == 0.0) {
    cerr << "result = " << result << endl;
    return -1;
  }

  if (grad.size() != basis.get_nparam()) {
    cerr << "gradient size = " << grad.size()
	 << " != BasisRule<7,Complex2>::get_nparam = " << basis.get_nparam()
	 << endl;
    return -1;
  }

  for (unsigned i=0; i < 7; i++)
    if (grad[i] != grad[i+7]) {
      cerr <<
	"gradient[" << i << "] = " << grad[i] << " != "
	"gradient[" << i+7 << "] = " << grad[i+7] << endl;
      return -1;
    }

  Matrix<7,7,double> xform;
  matrix_identity (xform);

  basis.set_transformation (xform);

  cerr << "BasisRule template passes all tests" << endl;
  return 0;
} 
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

