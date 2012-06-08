/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AuxColdPlasmaMeasures.h"

//! Default constructor
Pulsar::AuxColdPlasmaMeasures::AuxColdPlasmaMeasures ()
  : Extension ("AuxColdPlasmaMeausures")
{
  dispersion_measure = rotation_measure = 0.0;
}

Pulsar::AuxColdPlasmaMeasures* Pulsar::AuxColdPlasmaMeasures::clone () const
{
  return new AuxColdPlasmaMeasures (*this);
}

//! Set the dispersion measure
void Pulsar::AuxColdPlasmaMeasures::set_dispersion_measure (double dm)
{
  dispersion_measure = dm;
}

//! Get the dispersion measure
double Pulsar::AuxColdPlasmaMeasures::get_dispersion_measure () const
{
  return dispersion_measure;
}

//! Set the rotation measure
void Pulsar::AuxColdPlasmaMeasures::set_rotation_measure (double rm)
{
  rotation_measure = rm;
}

//! Get the rotation measure
double Pulsar::AuxColdPlasmaMeasures::get_rotation_measure () const
{
  return rotation_measure;
}

//! Return a text interfaces that can be used to access this instance
TextInterface::Parser* Pulsar::AuxColdPlasmaMeasures::get_interface()
{
  return new Interface( this );
}

