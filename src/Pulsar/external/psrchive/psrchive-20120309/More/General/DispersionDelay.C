/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/DispersionDelay.h"
#include "Physical.h"

using namespace std;

Pulsar::DispersionDelay::DispersionDelay () 
{
  dispersion_measure = 0.0;
  reference_wavelength = 0.0;
  wavelength = 0.0;
}

//! Set the dispersion measure
void Pulsar::DispersionDelay::set_dispersion_measure (const double dm)
{
  dispersion_measure = dm;
}

//! Get the dispersion measure
double Pulsar::DispersionDelay::get_dispersion_measure () const
{
  return dispersion_measure;
}

/*! This alias enables use of this class with the ColdPlasma template */
void Pulsar::DispersionDelay::set_measure (const double dm)
{
  dispersion_measure = dm;
}

/*! This alias enables use of this class with the ColdPlasma template */
double Pulsar::DispersionDelay::get_measure () const
{
  return dispersion_measure;
}

//! Set the reference frequency in MHz
void Pulsar::DispersionDelay::set_reference_frequency (double MHz)
{
  set_reference_wavelength( Pulsar::speed_of_light / (MHz * 1e6) );
}

//! Get the reference frequency in MHz
double Pulsar::DispersionDelay::get_reference_frequency () const
{
  return 1e-6 * Pulsar::speed_of_light / reference_wavelength;
}


//! Set the frequency in MHz
void Pulsar::DispersionDelay::set_frequency (double MHz)
{
  set_wavelength( Pulsar::speed_of_light / (MHz * 1e6) );
}

//! Get the frequency in MHz
double Pulsar::DispersionDelay::get_frequency () const
{
  return 1e-6 * Pulsar::speed_of_light / wavelength;
}

//! Set the reference wavelength in metres
void Pulsar::DispersionDelay::set_reference_wavelength (double metres)
{
  reference_wavelength = metres;
}

//! Get the reference wavelength in metres
double Pulsar::DispersionDelay::get_reference_wavelength () const
{
  return reference_wavelength;
}


//! Set the wavelength in metres
void Pulsar::DispersionDelay::set_wavelength (double metres)
{
  wavelength = metres;
}

//! Get the wavelength in metres
double Pulsar::DispersionDelay::get_wavelength () const
{
  return wavelength;
}

double Pulsar::DispersionDelay::evaluate () const
{
  // wavelength is expressed in light microseconds ...
  double lus_0 = reference_wavelength * 1e6 / speed_of_light;
  double lus = wavelength * 1e6 / speed_of_light;

  // ... in this typical equation
  return (dispersion_measure/2.41e-4) * (lus*lus - lus_0*lus_0);
}
