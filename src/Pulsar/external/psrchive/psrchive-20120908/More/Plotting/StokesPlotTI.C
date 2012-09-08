/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/StokesPlot.h"
#include "Pulsar/FluxPlot.h"

Pulsar::StokesPlot::Interface::Interface (StokesPlot* instance)
{
  if (instance)
    set_instance (instance);

  import ( FluxPlot::Interface() );

  remove ("pol");

  add( &StokesPlot::get_plot_values,
       &StokesPlot::set_plot_values,
       "val", "Values to be plotted (IQUVLp)" );

  add( &StokesPlot::get_plot_colours,
       &StokesPlot::set_plot_colours,
       "ci", "PGPLOT colour index for each value" );

  add( &StokesPlot::get_plot_lines,
       &StokesPlot::set_plot_lines,
       "ls", "PGPLOT line style for each value" );

  add( &StokesPlot::get_plot_widths,
       &StokesPlot::set_plot_widths,
       "lw", "PGPLOT line width for each value" );

  add( &StokesPlot::get_pav_bias,
       &StokesPlot::set_pav_bias,
       "pav", "Handle bias as done in pav" );

}
