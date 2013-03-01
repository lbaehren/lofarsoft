/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/Axis.h"
#include "MEAL/Polynomial.h"
#include "MEAL/RotatingVectorModel.h"
#include "MEAL/ProductRule.h"

using namespace std;

int main (int argc, char** argv) try {

  MEAL::Function::verbose = true;

  cerr << "Instantiating MEAL::Polynomial" << endl;
  MEAL::Polynomial poly (2);
  poly.set_param (1, 1.0);

  cerr << "Instantiating MEAL::RotatingVectorModel" << endl;
  MEAL::RotatingVectorModel rvm;

  cerr << "Instantiating MEAL::Axis<double>" << endl;
  MEAL::Axis<double> axis;

  cerr << "Connecting MEAL::Polynomial::set_abscissa"
    " to MEAL::Axis<double>" << endl;

  poly.set_argument (0, &axis);

  cerr << "Connecting MEAL::RotatingVectorModel::set_abscissa"
    " to MEAL::Axis<double>" << endl;

  rvm.set_argument (0, &axis);

  double test_value = 3.4;

  cerr << "Instantiating MEAL::Axis<double>::Value" << endl;

  MEAL::Argument::Value* abscissa = axis.new_Value (test_value);

  if (poly.get_abscissa() != 0.0) {
    cerr << "Error Polynomial::get_abscissa=" << poly.get_abscissa() << " != 0"
	 << endl;
    return -1;
  }

  cerr << "Calling MEAL::Axis<double>::apply" << endl;
  abscissa->apply();

  if (poly.get_abscissa() != test_value) {
    cerr << "Error Polynomial::get_abscissa=" << poly.get_abscissa() << " != "
	 << test_value << endl;
    return -1;
  }

  if (rvm.get_abscissa() != test_value) {
    cerr << "Error RotatingVectorModel::get_abscissa="
         << rvm.get_abscissa() << " != " << test_value << endl;
    return -1;
  }

  cerr << "Test if Composite class passes set_abscissa to components" << endl;

  cerr << "Instantiating MEAL::Polynomial" << endl;
  MEAL::Polynomial poly2 (2);

  cerr << "Instantiating MEAL::RotatingVectorModel" << endl;
  MEAL::RotatingVectorModel rvm2;

  MEAL::ProductRule<MEAL::Scalar> product;
  product.add_model (&poly2);
  product.add_model (&rvm2);
  product.set_argument (0, &axis);

  cerr << "Calling MEAL::Axis<double>::apply" << endl;
  abscissa->apply();

  if (poly2.get_abscissa() != test_value) {
    cerr << "Error Polynomial::get_abscissa=" << poly2.get_abscissa() << " != "
         << test_value << endl;
    return -1;
  }

  if (rvm2.get_abscissa() != test_value) {
    cerr << "Error RotatingVectorModel::get_abscissa="
         << rvm2.get_abscissa() << " != " << test_value << endl;
    return -1;
  }

  delete abscissa;

  cerr << "test successful" << endl;
  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}

