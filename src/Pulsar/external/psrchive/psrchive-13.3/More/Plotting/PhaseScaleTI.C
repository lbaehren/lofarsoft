/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PhaseScale.h"
#include "Pulsar/PlotScale.h"

Pulsar::PhaseScale::Interface::Interface (PhaseScale* instance)
{
  if (instance)
    set_instance (instance);

  add( &PhaseScale::get_units,
       &PhaseScale::set_units,
       "unit", "Units on phase axis (turn,deg,rad,ms)" );

  add( &PhaseScale::get_origin_norm,
       &PhaseScale::set_origin_norm,
       "origin", "Offset of origin on phase axis" );
}
