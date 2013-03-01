/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/PlotAxis.h"

Pulsar::PlotAxis::Interface::Interface (PlotAxis* instance)
{
  if (instance)
    set_instance (instance);

  add( &PlotAxis::get_label,
       &PlotAxis::set_label,
       "lab", "Label to be drawn on axis" );

  add( &PlotAxis::get_displacement,
       &PlotAxis::set_displacement,
       "disp", "Displacement between label and axis" );

  add( &PlotAxis::get_opt,
       &PlotAxis::set_opt,
       "opt", "Options passed to PGBOX" );

  add( &PlotAxis::get_tick,
       &PlotAxis::set_tick,
       "tick", "World coordinate interval between major ticks");
  
  add( &PlotAxis::get_nsub,
       &PlotAxis::set_nsub,
       "nsub", "Number of ticks between major ticks");

  add( &PlotAxis::get_alternate,
       &PlotAxis::set_alternate,
       "alt", "Permit alternate scale on opposite axis" );

}
