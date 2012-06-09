/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ColdPlasmaHistory.h"
#include "Physical.h"

//! Default constructor
Pulsar::ColdPlasmaHistory::ColdPlasmaHistory (const char* name)
  : Extension (name)
{
  measure = 0.0;
  reference_wavelength = 0.0;
}

//! Copy constructor
Pulsar::ColdPlasmaHistory::ColdPlasmaHistory (const ColdPlasmaHistory& cph)
  : Extension ("ColdPlasmaHistory")
{
  operator=(cph);
}

//! Operator =
const Pulsar::ColdPlasmaHistory&
Pulsar::ColdPlasmaHistory::operator= (const ColdPlasmaHistory& cph)
{
  measure = cph.measure;
  reference_wavelength = cph.reference_wavelength;

  return *this;
}

//! Destructor
Pulsar::ColdPlasmaHistory::~ColdPlasmaHistory ()
{
}

//! Set the rotation measure
void Pulsar::ColdPlasmaHistory::set_measure (double m)
{
  measure = m;
}

//! Get the rotation measure
double Pulsar::ColdPlasmaHistory::get_measure () const
{
  return measure;
}

//! Set the reference wavelength in metres
void Pulsar::ColdPlasmaHistory::set_reference_wavelength (double metres)
{
  reference_wavelength = metres;
}

//! Get the reference wavelength
double Pulsar::ColdPlasmaHistory::get_reference_wavelength () const
{
  return reference_wavelength;
}

//! Set the reference frequency in MHz
void Pulsar::ColdPlasmaHistory::set_reference_frequency (double MHz)
{
  reference_wavelength = speed_of_light / (MHz * 1e6);
}

//! Get the reference frequency in MHz
double Pulsar::ColdPlasmaHistory::get_reference_frequency () const
{
  return 1e-6 * speed_of_light / reference_wavelength;
}
