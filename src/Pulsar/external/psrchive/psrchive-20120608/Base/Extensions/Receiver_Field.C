/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Receiver_Field.h"

Pulsar::Receiver::Field::Field ()
{
  field_orientation = false;
}

//! Get the orientation of the basis about the line of sight
Angle Pulsar::Receiver::Field::get_orientation () const
{
  Angle offset;
  if (field_orientation && basis == Signal::Linear)
    offset.setDegrees (-45);

  return orientation + offset;
}

//! Set the orientation of the basis about the line of sight
void Pulsar::Receiver::Field::set_orientation (const Angle& angle)
{
  orientation = angle;
  field_orientation = false;
}

//! Get the orientation of the equal in-phase electric field vector
Angle Pulsar::Receiver::Field::get_field_orientation () const
{
  Angle offset = 0.0;
  if (!field_orientation && basis == Signal::Linear)
    offset.setDegrees (45.0);
  
  return orientation + offset;
}

//! Set the orientation of the equal in-phase electric field vector
void Pulsar::Receiver::Field::set_field_orientation (const Angle& angle)
{
  orientation = angle;
  field_orientation = true;
}
