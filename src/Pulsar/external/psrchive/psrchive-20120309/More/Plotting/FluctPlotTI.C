/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/FluctPlot.h"

Pulsar::FluctPlot::Interface::Interface (FluctPlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &FluctPlot::get_subint,
       &FluctPlot::set_subint,
       "subint", "Sub-integration to plot" );

  add( &FluctPlot::get_chan,
       &FluctPlot::set_chan,
       "chan", "Frequency channel to plot" );

  add( &FluctPlot::get_pol,
       &FluctPlot::set_pol,
       "pol", "Polarization to plot" );
}
