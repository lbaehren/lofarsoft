//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/MEAL/MEAL/GimbalLockMonitor.h,v $
   $Revision: 1.3 $
   $Date: 2007/11/05 00:26:09 $
   $Author: straten $ */

#ifndef __Pulsar_GimbalLockMonitor_h
#define __Pulsar_GimbalLockMonitor_h

#include "MEAL/Rotation1.h"

namespace MEAL {

  //! Monitors a rotation during least-squares minimization
  class GimbalLockMonitor : public Reference::Able {

  public:

    //! Constructor
    GimbalLockMonitor ();

    //! Set the yaw rotation
    void set_yaw (Rotation1*);

    //! Set the pitch rotation
    void set_pitch (Rotation1*);

    //! Set the roll rotation
    void set_roll (Rotation1*);

    //! Set the limits on the pitch rotation
    void set_pitch_limits (double lower, double upper);

    //! Return true if gimbal lock is detected
    bool lock_detected ();

  protected:

    //! The yaw rotation
    Reference::To<Rotation1> yaw;

    //! The pitch rotation
    Reference::To<Rotation1> pitch;

    //! The roll rotation
    Reference::To<Rotation1> roll;

    //! Flag set when gimbal lock detected
    bool gimbal_lock_detected;

    double lower_pitch_limit, upper_pitch_limit;
    double lower_yaw_limit, upper_yaw_limit;
  };

}

#endif
