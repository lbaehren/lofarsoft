/***************************************************************************
 *
 *   Copyright (C) 2006-2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseVsPlot.h"
#include "Pulsar/PhasePlot.h"
#include "Pulsar/PlotScale.h"
#include "pairutil.h"

Pulsar::PhaseVsPlot::Interface::Interface (PhaseVsPlot* instance)
{
  if (instance)
    set_instance (instance);

  // import the interface of the base class
  import( PhasePlot::Interface() );

  add( &PhaseVsPlot::get_rows,
       &PhaseVsPlot::set_rows,
       "rows", "Select rows to be plotted");

  import( "z", PlotScale::Interface(), &PhaseVsPlot::get_z_scale );

  add( &PhaseVsPlot::get_style,
       &PhaseVsPlot::set_style,
       "style", "Plot style: image or line");

  add( &PhaseVsPlot::get_crop,
       &PhaseVsPlot::set_crop,
       "crop", "Crop the data at this percentage of max" );

  import("cmap", pgplot::ColourMap::Interface(), &PhaseVsPlot::get_colour_map);

}
