/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/AnglePlot.h"
#include "Pulsar/PhasePlot.h"
#include "pairutil.h"

Pulsar::AnglePlot::Interface::Interface (AnglePlot* instance)
{
  if (instance)
    set_instance (instance);

  add( &AnglePlot::get_subint,
       &AnglePlot::set_subint,
       "subint", "Sub-integration to plot" );

  add( &AnglePlot::get_chan,
       &AnglePlot::set_chan,
       "chan", "Frequency channel to plot" );

  // import the interface of the base class
  import( PhasePlot::Interface() );

  add( &AnglePlot::get_marker,
       &AnglePlot::set_marker,
       "mark", "Marker used for each datum" );

  add( &AnglePlot::get_threshold,
       &AnglePlot::set_threshold,
       "cut", "Plot only points with R > cut * sigma_R" );

  add( &AnglePlot::get_span,
       &AnglePlot::set_span,
       "span", "Span of angles in plot (degrees)" );
  
}
