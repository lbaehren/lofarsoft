/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModel.h"
#include "MEAL/Coherency.h"
#include "MEAL/Polar.h"
#include "MEAL/Rotation.h"

using namespace std;

class TestComposite : public Calibration::ReceptionModel 
{
public:
  bool get_changed () const { return get_evaluation_changed(); }
};


int main ()
{
  // MEAL::Function::verbose = MEAL::Function::very_verbose = true;

  TestComposite model;

  MEAL::Coherency* coherency = new MEAL::Coherency;
  MEAL::Polar* polar = new MEAL::Polar;
  MEAL::Rotation* rotation = new MEAL::Rotation;

  MEAL::ProductRule<MEAL::Complex2> instrument1;
  instrument1 *= polar;

  MEAL::ProductRule<MEAL::Complex2> instrument2;
  instrument2 *= polar;
  instrument2 *= rotation;

  model.add_input (coherency);

  model.add_transformation (&instrument1);
  model.add_transformation (&instrument2);

  for (unsigned i=0; i < model.get_nparam(); i++)
    cerr << i << "=" << model.get_param_name (i) << endl;

  coherency->set_param (0, 1);
  coherency->set_param (1, 1/sqrt(2.0));

  polar->set_param (1, 0.1);  // b_1
  polar->set_param (5, 0.1);  // r_2

  Jones<double> instrument_result = instrument1.evaluate ();
  Jones<double> coherency_result = coherency->evaluate ();

  Jones<double> model_expect
    = instrument_result * coherency_result * herm(instrument_result);

  cerr << "expect=" << model_expect << endl;

  vector< Jones<double> > gradient;

  Jones<double> model_result = model.evaluate ( &gradient );

  cerr << "result=" << model_result << endl;

  cerr << "gradient.size=" << gradient.size() << endl;

  if (model_expect != model_result)
  {
    cerr <<
      " result=" << model_result << " != \n"
      " expect=" << model_expect << endl;

    return -1;
  }

  Jones<double> zero = 0;

  for (unsigned i=0; i < gradient.size(); i++)
    if ( gradient[i] == zero )
      {
	cerr << "gradient[" << model.get_param_name(i) << "] is zero" << endl;
      }

  cerr <<
    "\n"
    "\n"
    "test completed successfully \n"
    "\n"
    "\n";

  return 0;
}

