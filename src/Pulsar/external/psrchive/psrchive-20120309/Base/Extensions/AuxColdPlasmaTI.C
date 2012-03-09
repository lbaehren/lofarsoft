/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AuxColdPlasma.h"

Pulsar::AuxColdPlasma::Interface::Interface (AuxColdPlasma* a)
{
  if (a)
    set_instance (a);

  add( &AuxColdPlasma::get_dispersion_model_name,
       &AuxColdPlasma::set_dispersion_model_name,
       "dm_model", "Auxiliary dispersion model" );

  add( &AuxColdPlasma::get_dispersion_corrected,
       &AuxColdPlasma::set_dispersion_corrected,
       "dmc", "Auxiliary dispersion corrected" );

  add( &AuxColdPlasma::get_birefringence_model_name,
       &AuxColdPlasma::set_birefringence_model_name,
       "rm_model", "Auxiliary birefringence model" );

  add( &AuxColdPlasma::get_birefringence_corrected,
       &AuxColdPlasma::set_birefringence_corrected,
       "rmc", "Auxiliary birefringence corrected" );
}
