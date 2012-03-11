/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Horizon.h"

using namespace std;

int main (int argc, char** argv) try
{  
  Horizon horizon;

  cerr << "Simulating a telescope at lat=long=0 \n" << endl;
  horizon.set_observatory_latitude (0.0);
  horizon.set_observatory_longitude (0.0);

  //
  //
  //

  cerr << "For a source directly north of dish: \n"
       << "expect HA = LST - RA = 0 and PA = 180" << endl;

  sky_coord coordinates ("00:00+45:00");
  horizon.set_source_coordinates( coordinates );
  horizon.set_hour_angle (0.0);
  double pa = horizon.get_parallactic_angle() * 180/M_PI;

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << horizon.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa << " deg" << endl;

  if (horizon.get_parallactic_angle() != M_PI) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  //
  //
  //

  cerr << "\nFor a source north-west of dish: \n"
       << "expect HA = LST - RA > 90 and 0 < PA < 180" << endl;

  horizon.set_source_coordinates( coordinates );
  horizon.set_hour_angle (1.0);
  pa = horizon.get_parallactic_angle();

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << horizon.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa * 180/M_PI << " deg" << endl;

  if (pa < M_PI/2 || pa > M_PI) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  //
  //
  //

  cerr << "\nFor a source directly west of dish: \n"
       << "HA = LST - RA > 0 and PA=90" << endl;

  coordinates = sky_coord ("00:00+00:00");
  horizon.set_source_coordinates( coordinates );
  horizon.set_hour_angle (2.0);
  pa = horizon.get_parallactic_angle();

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << horizon.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa * 180/M_PI << " deg" << endl;

  //
  //
  //

  if (pa != M_PI/2) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  cerr << "\nFor a source south west of dish: \n"
       << "HA = LST - RA > 0 and 0 < PA < 90" << endl;

  coordinates = sky_coord ("00:00-45:00");
  horizon.set_source_coordinates( coordinates );
  horizon.set_hour_angle (1.0);
  pa = horizon.get_parallactic_angle();

  cerr << "source coordinates=" << coordinates << endl;
  cerr << "hour angle=" << horizon.get_hour_angle() << endl;
  cerr << "parallactic angle=" << pa * 180/M_PI << " deg" << endl;

  if (pa < 0 || pa > M_PI/2) {
    cerr << "Unexpected parallactic angle" << endl;
    return -1;
  }

  cerr << "\nCalibration::Parallactic passes test" << endl;
  return 0;

}
catch (Error& error) {
  cerr << error << endl;
  return -1;
}
