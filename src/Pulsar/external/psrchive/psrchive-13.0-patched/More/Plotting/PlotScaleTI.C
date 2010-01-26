/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotScale.h"
#include "pairutil.h"

Pulsar::PlotScale::Interface::Interface (PlotScale* instance)
{
  if (instance)
    set_instance (instance);

  import( PlotEdge::Interface() );

  add( &PlotScale::get_world,
       &PlotScale::set_world,
       "win", "World coordinate range inside viewport" );

  add( &PlotScale::get_world_external,
       &PlotScale::set_world_external,
       "wout", "World coordinate range outside viewport" );

  add( &PlotScale::get_range_norm,
       &PlotScale::set_range_norm,
       "range", "World-normalized range inside viewport" );

  add( &PlotScale::get_buf_norm,
       &PlotScale::set_buf_norm,
       "buf", "World-normalized buffer space" );
}
