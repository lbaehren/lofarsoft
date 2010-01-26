/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "MEAL/CyclicParameter.h"
#include "MEAL/Rotation1.h"

#include <iostream>
using namespace std;

int main () {

  MEAL::Rotation1* orientation;
  orientation = new MEAL::Rotation1 (Vector<3,double>::basis(2));

  // set up the cyclic boundary for orientation
  MEAL::CyclicParameter* o_cyclic = 0;
  o_cyclic = new MEAL::CyclicParameter (orientation);

  o_cyclic->set_period (M_PI);
  o_cyclic->set_upper_bound (M_PI/2);
  o_cyclic->set_lower_bound (-M_PI/2);
  
  orientation->set_parameter_policy (o_cyclic);
  
  MEAL::Rotation1* ellipticity;
  ellipticity = new MEAL::Rotation1 (Vector<3,double>::basis(1));

  // set up the cyclic boundary for ellipticity
  MEAL::CyclicParameter* e_cyclic = 0;
  e_cyclic = new MEAL::CyclicParameter (ellipticity);
  
  e_cyclic->set_period (M_PI);
  e_cyclic->set_upper_bound (M_PI/4);
  e_cyclic->set_lower_bound (-M_PI/4);
  e_cyclic->set_azimuth (o_cyclic);
  
  ellipticity->set_parameter_policy (e_cyclic);

  double test = 0;

  test = 0.76 * M_PI/2;
  cerr << "orientation set1" << endl;
  orientation->set_param (0, test);
  if (orientation->get_param(0) != test) {
    cerr << "orientation not set1" << endl;
    return -1;
  }

  test = 1.1 * M_PI/2;
  cerr << "orientation reset1" << endl;
  orientation->set_param (0, test);
  if (orientation->get_param(0) != test - M_PI) {
    cerr << "orientation not reset1" << endl;
    return -1;
  }

  test = -0.37 * M_PI/2;
  cerr << "orientation set2" << endl;
  orientation->set_param (0, test);
  if (orientation->get_param(0) != test) {
    cerr << "orientation not set2" << endl;
    return -1;
  }

  test = -5.4 * M_PI/2;
  cerr << "orientation reset2" << endl;
  orientation->set_param (0, test);
  if (orientation->get_param(0) != test + 3*M_PI) {
    cerr << "orientation not reset2" << endl;
    return -1;
  }

  test = M_PI/4;
  cerr << "orientation set before ellipticity test" << endl;
  orientation->set_param (0, test);

  test = 0.76 * M_PI/4;
  cerr << "ellipticity set1" << endl;
  ellipticity->set_param (0, test);
  if (ellipticity->get_param(0) != test) {
    cerr << "ellipticity not set1" << endl;
    return -1;
  }

  test = 1.1 * M_PI/4;
  cerr << "ellipticity reset1" << endl;
  ellipticity->set_param (0, test);
  if (fabs(ellipticity->get_param(0) - (0.5*M_PI - test)) > 1e-12) {
    cerr << "ellipticity not reset1" << endl;
    double out = ellipticity->get_param(0);
    cerr << "out=" << out << " exp=" << M_PI/2 - test << endl;
    return -1;
  }

  if (orientation->get_param(0) != -M_PI/4) {
    cerr << "ellipticity does not reset orientation1" << endl;
    return -1;
  }

  test = -0.37 * M_PI/4;
  cerr << "ellipticity set2" << endl;
  ellipticity->set_param (0, test);
  if (ellipticity->get_param(0) != test) {
    cerr << "ellipticity not set2" << endl;
    return -1;
  }

  test = -5*M_PI/8;
  cerr << "ellipticity reset2" << endl;
  ellipticity->set_param (0, test);
  if (ellipticity->get_param(0) != M_PI/8) {
    cerr << "ellipticity not reset2" << endl;
    double out = ellipticity->get_param(0);
    cerr << "out=" << out << " exp=" << M_PI/8 << endl;
    return -1;
  }

  if (orientation->get_param(0) != M_PI/4) {
    cerr << "ellipticity does not reset orientation2" << endl;
    return -1;
  }

  cerr << "CyclicParameter checks passed" << endl;
  return 0;
}
