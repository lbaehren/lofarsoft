/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesFluctPlot.h"
#include "Pulsar/FluctPlot.h"

Pulsar::StokesFluctPlot::Interface::Interface (StokesFluctPlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluctPlot::Interface() );

  remove ("pol");

  add( &StokesFluctPlot::get_plot_last_harmonic,
       &StokesFluctPlot::set_plot_last_harmonic,
       "last", "Plot the last harmonic marker" );

  add( &StokesFluctPlot::get_plot_values,
       &StokesFluctPlot::set_plot_values,
       "val", "Values to be plotted (IQUVLpS)" );

  add( &StokesFluctPlot::get_plot_colours,
       &StokesFluctPlot::set_plot_colours,
       "sci", "PGPLOT colour index for each value" );
 
  add( &StokesFluctPlot::get_plot_lines,
       &StokesFluctPlot::set_plot_lines,
       "sls", "PGPLOT line style for each value" );

}
