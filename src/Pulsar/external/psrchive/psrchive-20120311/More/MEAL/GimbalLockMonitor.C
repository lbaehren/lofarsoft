/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/GimbalLockMonitor.h"

#include <iostream>
using namespace std;

MEAL::GimbalLockMonitor::GimbalLockMonitor ()
{
  gimbal_lock_detected = false;

  upper_pitch_limit = M_PI / 8;
  lower_pitch_limit = -upper_pitch_limit;

  upper_yaw_limit = M_PI / 4;
  lower_yaw_limit = -upper_yaw_limit;
}

//! Set the yaw rotation to be monitored
void MEAL::GimbalLockMonitor::set_yaw (MEAL::Rotation1* r)
{
  yaw = r;
}

//! Set the pitch rotation to be monitored
void MEAL::GimbalLockMonitor::set_pitch (MEAL::Rotation1* r)
{
  pitch = r;
}

//! Set the roll rotation to be monitored
void MEAL::GimbalLockMonitor::set_roll (MEAL::Rotation1* r)
{
  roll = r;
}

//! Set the limits on the rotation
void MEAL::GimbalLockMonitor::set_pitch_limits (double lower, double upper)
{
  lower_pitch_limit = lower;
  upper_pitch_limit = upper;
}

//! Return true if gimbal lock is detected
bool MEAL::GimbalLockMonitor::lock_detected ()
{
  double pitch_rad = pitch->get_param(0);
  // double yaw_rad = yaw->get_param(0);

  if (!gimbal_lock_detected &&
      ( pitch_rad < lower_pitch_limit || pitch_rad > upper_pitch_limit ))
  {

    gimbal_lock_detected = true;

    cerr << endl << 
      "gimbal lock detected"
	 << endl <<
      " yaw=" << yaw->get_param(0) * 180/M_PI << " deg\n"
      " pitch=" << pitch->get_param(0) * 180/M_PI << " deg\n" 
      " roll=" << roll->get_param(0) * 180/M_PI << " deg\n" 
	 << endl;

    cerr <<
      "It is possible that the required transformation includes a reflection" 
	 << endl << endl;;

  }

  return gimbal_lock_detected;
}

    
