 /***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationMeta.h"
#include "Pulsar/AuxColdPlasma.h"
#include "sky_coord.h"

//! Construct from the parent Archive instance
Pulsar::Integration::Meta::Meta (const Archive* parent)
{

  set_telescope( parent->get_telescope() );
  set_coordinates( parent->get_coordinates() );

  set_centre_frequency( parent->get_centre_frequency() );
  set_bandwidth( parent->get_bandwidth() );
  set_dispersion_measure( parent->get_dispersion_measure() );
  set_dedispersed( parent->get_dedispersed() );

  set_rotation_measure( parent->get_rotation_measure() );
  set_faraday_corrected( parent->get_faraday_corrected() );

  const AuxColdPlasma* aux = parent->get<AuxColdPlasma> ();
  if (aux)
  {
    set_auxiliary_dispersion_corrected (aux->get_dispersion_corrected());
    set_auxiliary_birefringence_corrected (aux->get_birefringence_corrected());
  }
  else
  {
    set_auxiliary_dispersion_corrected (false);
    set_auxiliary_birefringence_corrected (false);
  }

  set_state( parent->get_state() );
  set_basis( parent->get_basis() );
}

std::string Pulsar::Integration::Meta::get_telescope() const
{
  return telescope;
}

void Pulsar::Integration::Meta::set_telescope(std::string name)
{
  telescope = name;
}

sky_coord Pulsar::Integration::Meta::get_coordinates() const
{
  return coordinates;
}

void Pulsar::Integration::Meta::set_coordinates(const sky_coord &c)
{
  coordinates = c;
}

//! Get the centre frequency (in MHz)
double Pulsar::Integration::Meta::get_centre_frequency() const
{
  return centre_frequency;
}

void Pulsar::Integration::Meta::set_centre_frequency (double v)
{
  centre_frequency = v;
}

//! Get the bandwidth (in MHz)
double Pulsar::Integration::Meta::get_bandwidth() const
{
  return bandwidth;
}

void Pulsar::Integration::Meta::set_bandwidth (double v)
{
  bandwidth = v;
}

//! Get the dispersion measure (in \f${\rm pc\, cm}^{-3}\f$)
double Pulsar::Integration::Meta::get_dispersion_measure () const
{
  return dispersion_measure;
}

void Pulsar::Integration::Meta::set_dispersion_measure (double v)
{
  dispersion_measure = v;
}

//! Inter-channel dispersion delay has been removed
bool Pulsar::Integration::Meta::get_dedispersed () const
{
  return dedispersed;
}

void Pulsar::Integration::Meta::set_dedispersed (bool v)
{
  dedispersed = v;
}

//! Get the rotation measure (in \f${\rm rad\, m}^{-2}\f$)
double Pulsar::Integration::Meta::get_rotation_measure () const
{
  return rotation_measure;
}

void Pulsar::Integration::Meta::set_rotation_measure (double v)
{
  rotation_measure = v;
}

//! Data has been corrected for ISM faraday rotation
bool Pulsar::Integration::Meta::get_faraday_corrected () const
{
  return faraday_corrected;
}

void Pulsar::Integration::Meta::set_faraday_corrected (bool v)
{
  faraday_corrected = v;
}

//! Get the feed configuration of the receiver
Signal::Basis Pulsar::Integration::Meta::get_basis () const
{
  return basis;
}

void Pulsar::Integration::Meta::set_basis (Signal::Basis v)
{
  basis = v;
}

//! Get the polarimetric state of the profiles
Signal::State Pulsar::Integration::Meta::get_state () const
{
  return state;
}

void Pulsar::Integration::Meta::set_state (Signal::State v)
{
  state = v;
}

//! Auxiliary inter-channel dispersion delay has been removed
bool Pulsar::Integration::Meta::get_auxiliary_dispersion_corrected () const
{
  return auxiliary_dispersion_corrected;
}

void Pulsar::Integration::Meta::set_auxiliary_dispersion_corrected (bool flag)
{
  auxiliary_dispersion_corrected = flag;
}

//! Auxiliary inter-channel birefringence has been removed
bool Pulsar::Integration::Meta::get_auxiliary_birefringence_corrected () const
{
  return auxiliary_birefringence_corrected;
}

void Pulsar::Integration::Meta::set_auxiliary_birefringence_corrected (bool f)
{
  auxiliary_birefringence_corrected = f;
}
