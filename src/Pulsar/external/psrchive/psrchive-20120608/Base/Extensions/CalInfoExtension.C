/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/CalInfoExtension.h"

//! Default constructor
Pulsar::CalInfoExtension::CalInfoExtension ()
  : Extension ("CalInfoExtension")
{
  cal_mode      = "UNSET";
  cal_frequency = 0.0;
  cal_dutycycle = 0.0;
  cal_phase     = 0.0;
  cal_nstate    = 2;
}

//! Copy constructor
Pulsar::CalInfoExtension::CalInfoExtension (const CalInfoExtension& extension)
  : Extension ("CalInfoExtension")
{
  operator = (extension);
}

//! Operator =
const Pulsar::CalInfoExtension&
Pulsar::CalInfoExtension::operator= (const CalInfoExtension& extension)
{
  cal_mode = extension.cal_mode;
  cal_frequency = extension.cal_frequency;
  cal_dutycycle = extension.cal_dutycycle;
  cal_phase = extension.cal_phase;
  cal_nstate = extension.cal_nstate;

  return *this;
}

//! Destructor
Pulsar::CalInfoExtension::~CalInfoExtension ()
{
}
