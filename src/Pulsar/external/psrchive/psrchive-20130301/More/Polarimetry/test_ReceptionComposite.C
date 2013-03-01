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
  MEAL::Function::cache_results = true;
  MEAL::Function::verbose = true;
  MEAL::Function::very_verbose = true;

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

  if (!model.get_changed())
  {
    cerr << "composite not changed by addition of components" << endl;
    return -1;
  }

  model.evaluate ();

  if (model.get_changed())
  {
    cerr << "composite changed after evaluation" << endl;
    return -1;
  }

  cerr << "setting component parameter" << endl;
  rotation->set_param (1, 3.4);

  if (!model.get_changed())
  {
    cerr << "composite not changed by modification of component" << endl;
    return -1;
  }

  cerr <<
    "\n"
    "\n"
    "test completed successfully \n"
    "\n"
    "\n";

  return 0;
}

