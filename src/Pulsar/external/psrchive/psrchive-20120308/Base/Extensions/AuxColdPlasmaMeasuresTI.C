/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/AuxColdPlasmaMeasures.h"

Pulsar::AuxColdPlasmaMeasures::Interface::Interface (AuxColdPlasmaMeasures* a)
{
  if (a)
    set_instance (a);

  add( &AuxColdPlasmaMeasures::get_dispersion_measure,
       &AuxColdPlasmaMeasures::set_dispersion_measure,
       "dm", "Auxiliary dispersion measure" );

  add( &AuxColdPlasmaMeasures::get_rotation_measure,
       &AuxColdPlasmaMeasures::set_rotation_measure,
       "rm", "Auxiliary rotation measure" );
}
