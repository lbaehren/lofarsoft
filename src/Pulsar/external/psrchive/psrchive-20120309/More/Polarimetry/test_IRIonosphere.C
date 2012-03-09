/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/IRIonosphere.h"

#include <iostream>
using namespace std;

int main ()
{
  Horizon horizon;

  horizon.set_observatory_longitude (150 * M_PI/180);
  horizon.set_observatory_latitude  (-33 * M_PI/180);
  horizon.set_epoch (52690.39);

  horizon.set_source_coordinates (sky_coord("07:38:32.4-40:42:41.1"));

  Calibration::IRIonosphere ionosphere;

  ionosphere.set_horizon (horizon);
  cerr << "RM = " << ionosphere.get_rotation_measure() << endl;

  return 0;
}
